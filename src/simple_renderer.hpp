#ifndef SIMPLE_RENDERER_HPP
#define SIMPLE_RENDERER_HPP

#include "camera.hpp"
#include "util/util.hpp"

#include <fstream>
#include <string>
#include <vector>

struct Sphere {
  Point3 center;
  double radius;
  Color color;
};

struct Scene {
  std::vector<Sphere> objects;
};

Color ray_color(const Ray& r, const Scene& scene);

void render_ppm(const Scene& scene,
                const Camera& cam,
                const std::string& filename);

#endif
