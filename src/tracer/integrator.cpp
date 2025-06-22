#include "integrator.hpp"

#include "bsdf.hpp"
#include "bxdfs/lambertian.hpp"
#include "light.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene.hpp"
#include "texture.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <random>
#include <thread>
#include <vector>
#include "spdlog/spdlog.h"

static inline Float absCosTheta(Vector3 wi, Normal n) {
  return std::abs(wi.Dot(n));
}

static constexpr Float PowerHeuristic(Float pdf_a, Float pdf_b) {
  Float a2 = pdf_a * pdf_a;
  Float b2 = pdf_b * pdf_b;
  return (a2 == 0.0 && b2 == 0.0) ? 0.0 : a2 / (a2 + b2);
}

Integrator::Integrator(Scene& scene, int max_depth, bool mis_enabled)
    : scene_(scene),
      max_depth_(max_depth),
      mis_enabled_(mis_enabled),
      ray_cnt_(0) {
  for (auto& obj : scene_.objs_) {
    if (obj->GetLight())
      lights_.push_back(obj);
  }
}

Color Integrator::Li(Ray r, int depth) {
  if (depth >= max_depth_)
    return Color(0);

  HitRecord rec = scene_.Hit(r, Interval<Float>::Positive());
  if (!rec.hits)
    return scene_.Background(r);

  Color L = rec.primitive->Le(r);

  std::shared_ptr<IMaterial> mat = rec.primitive->GetMaterial();
  while (mat) {
    if (auto mix = std::dynamic_pointer_cast<MixedMaterial>(mat))
      mat = mix->Select();
    else
      break;
  }
  if (!mat)
    return L;

  BSDF bsdf = mat->GetBSDF(rec);
  bool use_mis = !bsdf.MatchesFlag(BxDFBits::Specular) && mis_enabled_;

  // Direct lighting via light sampling
  auto sample_light = [&]() -> Color {
    static constexpr Float EPS = 1e-6;

    // randomly pick one light
    size_t idx = std::min<size_t>(random_uniform_01() * lights_.size(),
                                  lights_.size() - 1);
    auto light_prim = lights_[idx];
    auto shape = light_prim->GetShape();

    ShapeSample samp = shape->Sample();
    if (samp.pdf <= EPS)
      return Color(0);
    Vector3 wo = samp.pos - rec.position;
    Float dist_sq = wo.Length_squared();
    wo = wo.Normalized();
    const Float shading_cos = absCosTheta(wo, rec.normal);
    const Float light_cos = absCosTheta(-wo, samp.normal);
    const Float pdf_light =
        (1.0 / lights_.size()) * (samp.pdf * dist_sq / light_cos);
    if (shading_cos < EPS || light_cos < EPS)
      return Color(0);

    Ray shadow(rec.position, wo);
    HitRecord sh = scene_.Hit(shadow, Interval<Float>::Positive());
    if (!sh.hits || sh.primitive != light_prim.get())
      return Color(0);

    Color Li_light = light_prim->Le(shadow);
    const Float pdf_bsdf = bsdf.pdf(r.Direction(), wo);
    const Float w = PowerHeuristic(pdf_light, pdf_bsdf);
    Color L =
        bsdf.f(r.Direction(), wo) * Li_light * shading_cos * w / pdf_light;
    return L;
  };

  if (lights_.empty())
    use_mis = false;
  if (use_mis)
    L += sample_light();

  if (auto samp = bsdf.Sample_f(r.Direction())) {
    const Float pdf_bsdf = samp->pdf;
    const Float cos0 = absCosTheta(samp->wo, rec.normal);

    if (pdf_bsdf > 0.0 && cos0 > 0.0) {
      Float w = 1.0;
      if (use_mis) {
        Float pdf_light = 0;
        for (const auto& it : lights_)
          pdf_light += it->GetShape()->Pdf(rec.position, samp->wo);
        w = PowerHeuristic(pdf_bsdf, pdf_light);
      }

      Color throughput = samp->f * cos0 * w / pdf_bsdf;
      if (depth > 5) {
        Float survive =
            std::max({throughput.r(), throughput.g(), throughput.b()});
        survive = std::min(survive, 0.95);
        if (random_uniform_01() > survive)
          return L;
        else
          throughput /= survive;
      }

      Ray scattered(rec.position, samp->wo);
      const Color Li_scatter = Li(scattered, depth + 1);

      L += throughput * Li_scatter;
    }
  }

  return L;
}

void Integrator::Render(const Camera& cam,
                        std::string output_filename,
                        const int spp) {
  int image_width = cam.imageWidth();
  int image_height = cam.imageHeight();
  auto view = cam.initializeView();
  Point3 origin = cam.position();

  // Simple RGB‐byte framebuffer
  struct Pixel {
    uint8_t r, g, b;
  };
  std::vector<Pixel> framebuffer(image_width * image_height);

  unsigned num_threads = std::thread::hardware_concurrency();
  if (num_threads == 0)
    num_threads = 4;

  // Render a range of rows [y_start, y_end)
  std::atomic<int> finished_cnt = 0;
  auto render_rows = [&](int y_start, int y_end) {
    for (int y = y_start; y < y_end; ++y) {
      for (int x = 0; x < image_width; ++x) {
        // base point on the film
        Point3 pixel_center =
            view.pixel00_loc + view.pixel_delta_u * x + view.pixel_delta_v * y;

        Color raw(0, 0, 0);
        for (int i = 0; i < spp; ++i) {
          Point3 jittered = pixel_center +
                            random_uniform_01() * view.pixel_delta_u +
                            random_uniform_01() * view.pixel_delta_v;
          Ray r(origin, jittered - origin);
          raw += Li(r, 0);
          ++ray_cnt_;
        }
        raw /= spp;

        Color rgb = Format_Color(raw, 255.999);
        static const Interval<int> col_range(0, 255);
        Pixel p;
        p.r = static_cast<uint8_t>(col_range.Clamp(static_cast<int>(rgb.r())));
        p.g = static_cast<uint8_t>(col_range.Clamp(static_cast<int>(rgb.g())));
        p.b = static_cast<uint8_t>(col_range.Clamp(static_cast<int>(rgb.b())));
        framebuffer[y * image_width + x] = p;
      }

      spdlog::info("finished {}/{}", ++finished_cnt, image_height);
    }
  };

  // Spawn threads
  std::vector<std::thread> threads;
  int rows_per_thread = image_height / num_threads;
  int y0 = 0;
  for (unsigned i = 0; i < num_threads; ++i) {
    int y1 = (i + 1 == num_threads) ? image_height : y0 + rows_per_thread;
    threads.emplace_back(render_rows, y0, y1);
    y0 = y1;
  }

  // Join them
  for (auto& t : threads)
    t.join();

  // Dump to PPM
  std::ofstream out(output_filename);
  out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int y = 0; y < image_height; ++y) {
    for (int x = 0; x < image_width; ++x) {
      const Pixel& p = framebuffer[y * image_width + x];
      out << static_cast<int>(p.r) << ' ' << static_cast<int>(p.g) << ' '
          << static_cast<int>(p.b) << '\n';
    }
  }
}
