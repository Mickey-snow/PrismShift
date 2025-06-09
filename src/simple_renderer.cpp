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
                const std::string& filename) {
  // get size & precompute the viewing frustum
  int image_width = cam.imageWidth();
  int image_height = cam.imageHeight();
  auto view = cam.initializeView();
  Point3 origin = cam.position();

  std::ofstream out(filename);
  out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  // now iterate in row-major from top-left pixel (0,0) downwards
  for (int y = 0; y < image_height; ++y) {
    for (int x = 0; x < image_width; ++x) {
      // compute the exact center of pixel (x,y)
      Point3 pixel_center =
          view.pixel00_loc + view.pixel_delta_u * x + view.pixel_delta_v * y;

      Ray r(origin, pixel_center - origin);
      Color raw = ray_color(r, scene);
      Color rgb = Format_Color(raw, 255.999);
      out << int(rgb.x()) << ' ' << int(rgb.y()) << ' ' << int(rgb.z()) << '\n';
    }
  }
}
