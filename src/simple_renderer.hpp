#ifndef SIMPLE_RENDERER_HPP
#define SIMPLE_RENDERER_HPP

#include <fstream>
#include <string>
#include <vector>
#include "tracer/util/util.hpp"

struct Sphere {
  Point3 center;
  double radius;
  Color color;
};

struct Scene {
  std::vector<Sphere> objects;
};

struct Camera {
  Point3 origin;
  Vector3 horizontal;
  Vector3 vertical;
  Point3 lower_left_corner;

  explicit Camera(double aspect_ratio = 16.0 / 9.0) {
    double viewport_height = 2.0;
    double viewport_width = viewport_height * aspect_ratio;
    double focal_length = 1.0;

    origin = Point3(0, 0, 0);
    horizontal = Vector3(viewport_width, 0, 0);
    vertical = Vector3(0, viewport_height, 0);
    lower_left_corner =
        origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focal_length);
  }

  Ray get_ray(double u, double v) const {
    return Ray(origin,
               lower_left_corner + u * horizontal + v * vertical - origin);
  }
};

Color ray_color(const Ray& r, const Scene& scene);

void render_ppm(const Scene& scene,
                const Camera& cam,
                int image_width,
                int image_height,
                const std::string& filename);

#endif
