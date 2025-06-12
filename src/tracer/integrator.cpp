#include "integrator.hpp"

#include "bsdf.hpp"
#include "bxdfs/lambertian.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene.hpp"
#include "texture.hpp"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <random>
#include <thread>
#include <vector>

static inline double absCosTheta(Vector3 wi, Normal n) {
  return std::abs(wi.Dot(n));
}

Color Integrator::Li(Ray r, int depth) {
  if (depth >= max_depth_)
    return {};

  auto rec = scene_.Hit(r, Interval<double>::Positive());
  if (!rec.hits)
    return scene_.Background(r);

  Color L;
  BSDF bsdf(rec);
  auto bsdf_sample = bsdf.Sample_f(rec.ray.Direction());
  if (bsdf_sample && bsdf_sample->pdf > 0.0) {
    double cos0 = absCosTheta(bsdf_sample->wo, rec.normal);
    Ray scattered(rec.position, bsdf_sample->wo);
    scattered.origin = scattered.At(1e-6);
    Color Li_scatter = Li(scattered, depth + 1);
    L += bsdf_sample->f * Li_scatter * cos0 / bsdf_sample->pdf;
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

  // Random number setup
  std::random_device rd;

  unsigned num_threads = std::thread::hardware_concurrency();
  if (num_threads == 0)
    num_threads = 4;

  // Render a range of rows [y_start, y_end)
  auto render_rows = [&](int y_start, int y_end) {
    // Each thread gets its own RNG
    thread_local std::mt19937 gen(
        rd() ^ (std::mt19937::result_type)std::hash<std::thread::id>()(
                   std::this_thread::get_id()));
    std::uniform_real_distribution<double> distr(0.0, 1.0);

    for (int y = y_start; y < y_end; ++y) {
      for (int x = 0; x < image_width; ++x) {
        // base point on the film
        Point3 pixel_center =
            view.pixel00_loc + view.pixel_delta_u * x + view.pixel_delta_v * y;

        Color raw(0, 0, 0);
        for (int i = 0; i < spp; ++i) {
          Point3 jittered = pixel_center + distr(gen) * view.pixel_delta_u +
                            distr(gen) * view.pixel_delta_v;
          Ray r(origin, jittered - origin);
          raw += Li(r);
        }
        raw /= spp;

        Color rgb = Format_Color(raw, 255.999);
        static Interval<int> col_range(0, 255);
        Pixel p;
        p.r = static_cast<uint8_t>(col_range.Clamp(static_cast<int>(rgb.r())));
        p.g = static_cast<uint8_t>(col_range.Clamp(static_cast<int>(rgb.g())));
        p.b = static_cast<uint8_t>(col_range.Clamp(static_cast<int>(rgb.b())));
        framebuffer[y * image_width + x] = p;
      }
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
