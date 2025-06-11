#include "integrator.hpp"

#include "bsdf.hpp"
#include "bxdfs/lambertian.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "texture.hpp"

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
