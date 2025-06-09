#include "simple_renderer.hpp"

#include <cmath>

Color ray_color(const Ray& r, const Scene& scene) {
  double t_min = 0.001;
  double closest_t = infinity;
  Color col(0, 0, 0);
  bool hit_any = false;
  for (const auto& s : scene.objects) {
    Vector3 oc = r.Origin() - s.center;
    double a = r.Direction().Length_squared();
    double half_b = Vector3::Dot(oc, r.Direction());
    double c = oc.Length_squared() - s.radius * s.radius;
    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
      continue;
    double sqrtd = std::sqrt(discriminant);
    double root = (-half_b - sqrtd) / a;
    if (root < t_min || root > closest_t) {
      root = (-half_b + sqrtd) / a;
      if (root < t_min || root > closest_t)
        continue;
    }
    closest_t = root;
    Point3 p = r.At(root);
    Normal n = (Normal)((p - s.center) / s.radius);
    Color shade = Color(n.x() + 1, n.y() + 1, n.z() + 1) * 0.5;
    col = Color(shade.x() * s.color.x(), shade.y() * s.color.y(),
                shade.z() * s.color.z());
    hit_any = true;
  }
  if (hit_any)
    return col;
  Vector3 unit_direction = r.Direction().Normalized();
  double t = 0.5 * (unit_direction.y() + 1.0);
  Color a = Color(1.0, 1.0, 1.0);
  Color b = Color(0.5, 0.7, 1.0);
  a = Color(a.x() * (1.0 - t), a.y() * (1.0 - t), a.z() * (1.0 - t));
  b = Color(b.x() * t, b.y() * t, b.z() * t);
  return Color(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}

void render_ppm(const Scene& scene,
                const Camera& cam,
                int image_width,
                int image_height,
                const std::string& filename) {
  std::ofstream out(filename);
  out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int j = image_height - 1; j >= 0; --j) {
    for (int i = 0; i < image_width; ++i) {
      double u = double(i) / (image_width - 1);
      double v = double(j) / (image_height - 1);
      Ray r = cam.get_ray(u, v);
      Color pixel = ray_color(r, scene);
      Color formatted = Format_Color(pixel, 255.999);
      out << static_cast<int>(formatted.x()) << ' '
          << static_cast<int>(formatted.y()) << ' '
          << static_cast<int>(formatted.z()) << '\n';
    }
  }
}
