#include "integrator.hpp"

#include "bsdf.hpp"
#include "material.hpp"
#include "scene.hpp"

Color Integrator::Li(Ray r, int depth) {
  if (depth >= max_depth_)
    return {};

  auto rec = scene_.Hit(r, Interval<double>::Positive());
  if (!rec.hits)
    return scene_.Background(r);

  return {};
}
