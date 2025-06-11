#include "integrator.hpp"

#include "bsdf.hpp"
#include "bxdfs/lambertian.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene.hpp"
#include "texture.hpp"

#include <fstream>

static inline double absCosTheta(Vector3 wi, Normal n) {
  return std::abs(wi.Dot(n));
}

Color Integrator::Li(Ray r, int depth) {
  if (depth >= max_depth_)
    return {};

  auto rec = scene_.Hit(r, Interval<double>::Positive());
  if (!rec.hits)
    return scene_.Background(r);

  Material const* mat = rec.material;

  // TODO: sample emission
  Color L;

  // TODO: texture coordinate and mapping
  Point2 uv(0, 0);

  // Sample material parameters
  Color kd = mat->diffuse->Evaluate(uv);
  BSDF bsdf(std::make_shared<bxdfs::Lambertian>(kd));

  // Indirect reflection via BSDF sampling
  auto bsdf_sample = bsdf.Sample_f(r.Direction());
  if (bsdf_sample && bsdf_sample->pdf > 0.0) {
    double cos0 = absCosTheta(bsdf_sample->wo, rec.normal);

    Ray scattered(rec.position, bsdf_sample->wo);
    scattered.origin = scattered.At(1e-6);
    Color Li_scatter = Li(scattered, depth + 1);

    L += bsdf_sample->f * Li_scatter * cos0 / bsdf_sample->pdf;
  }

  return L;
}

void Integrator::Render(const Camera& cam, std::string output_filename) {
  int image_width = cam.imageWidth(), image_height = cam.imageHeight();
  auto view = cam.initializeView();
  Point3 origin = cam.position();

  std::ofstream out(output_filename);
  out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int y = 0; y < image_height; ++y) {
    for (int x = 0; x < image_width; ++x) {
      Point3 pixel_center =
          view.pixel00_loc + view.pixel_delta_u * x + view.pixel_delta_v * y;

      Color raw(0, 0, 0);
      for (int i = 0; i < 32; ++i) {
        Point3 center = pixel_center +
                        random_uniform_01() * view.pixel_delta_u +
                        random_uniform_01() * view.pixel_delta_v;

        Ray r(origin, center - origin);
        raw += Li(r);
      }
      raw /= 32;
      Color rgb = Format_Color(raw, 255.999);
      static Interval<int> col_range(0, 255);
      out << col_range.Clamp(static_cast<int>(rgb.r())) << ' '
          << col_range.Clamp(static_cast<int>(rgb.g())) << ' '
          << col_range.Clamp(static_cast<int>(rgb.b())) << '\n';
    }
  }
}
