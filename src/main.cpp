#include "integrator.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene.hpp"
#include "shapes/shape.hpp"
#include "texture.hpp"
#include "util/util.hpp"

Camera Get_Camera() {
  int image_h = 360;
  double aspect = 16.0 / 9.0;
  double vFovDeg = 45;
  // Camera(center, lookAt, imageHeight, aspect, verticalFovDegrees)
  Camera cam({12, 2, -2.5}, {0, 0, 0}, image_h, aspect, vFovDeg);
  return cam;
}

inline static std::shared_ptr<Primitive> make_sphere(Color color,
                                                     Point3 pos,
                                                     double d) {
  static const auto sphere = std::make_shared<Sphere>();

  auto mat = std::make_shared<Material>(
      std::make_shared<SolidColor>(std::move(color)));
  auto trans = std::make_shared<CompositeTransformation>(
      std::make_shared<VectorScale>(d, d, d),
      std::make_shared<VectorTranslate>(Vector3(pos)));
  // Note: transformation order is equivalent to
  // MatrixTransformation::Translate(Vector3(pos)) *
  //     MatrixTransformation::Scale(d, d, d)

  return std::make_shared<Primitive>(sphere, std::move(mat), std::move(trans));
}

Scene Get_World() {
  std::vector<std::shared_ptr<Primitive>> objs;

  // ground
  objs.emplace_back(
      make_sphere(Color(0.5, 0.5, 0.5), Point3(0, -1000, 0), 1000));
  for (int a = -7; a < 7; ++a)
    for (int b = -7; b < 7; ++b) {
      Point3 center(a + random_double(0, 0.9), 0.2, b + random_double(0, 0.9));

      if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
        auto albedo = Color(random_double(0.65, 1), random_double(0.65, 1),
                            random_double(0.65, 1));
        objs.emplace_back(make_sphere(albedo, center, 0.2));
      }
    }

  // auto material1 = make_shared<dielectric>(1.5);
  //   world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  objs.emplace_back(make_sphere(Color(0.2, 0.8, 0.3), Point3(0, 1.6, 1), 1.6));
  objs.emplace_back(make_sphere(Color(0.4, 0.2, 0.1), Point3(2, 1.6, 1), 1.6));
  objs.emplace_back(make_sphere(Color(0.4, 0.1, 0.9), Point3(4, 1.6, 1), 1.6));

  // auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  // world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  Scene scene(std::move(objs));
  scene.SetBackground(  // simple blue to white gradient
      [](Ray r) -> Color {
        auto direction = r.direction.normalised();
        double a = 0.5 * (1.0 + direction.y());
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
      });
  return scene;
}

int main() {
  auto camera = Get_Camera();
  auto scene = Get_World();
  Integrator integrator(scene, 32);
  integrator.Render(camera, "output.ppm", 128);
}
