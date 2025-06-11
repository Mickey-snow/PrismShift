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
  double vFovDeg = 60.0;
  // Camera(center, lookAt, imageHeight, aspect, verticalFovDegrees)
  Camera cam({12, 2, 3}, {0, 0, 0}, image_h, aspect, vFovDeg);
  return cam;
}

inline static std::shared_ptr<ITransformation> translate(double x,
                                                         double y,
                                                         double z) {
  return std::make_shared<VectorTranslate>(x, y, z);
}
inline static std::shared_ptr<ITransformation> translate(vector_like auto v) {
  return translate(v.x(), v.y(), v.z());
}
inline static Color rand_color(double l = 0, double r = 1) {
  return Color(random_double(l, r), random_double(l, r), random_double(l, r));
}

Scene Get_World() {
  std::vector<std::shared_ptr<Primitive>> objs;
  auto sphere = std::make_shared<Sphere>();

  auto ground_mat =
      std::make_shared<Material>(std::make_shared<SolidColor>(0.5, 0.5, 0.5));
  objs.emplace_back(
      std::make_shared<Primitive>(sphere, ground_mat, translate(0, -1000, 0)));
  for (int a = -11; a < 11; ++a)
    for (int b = -11; b < 11; ++b) {
      Point3 center(a + random_double(0, 0.9), 0.2, b + random_double(0, 0.9));

      if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
        auto albedo = rand_color();
        auto mat =
            std::make_shared<Material>(std::make_shared<SolidColor>(albedo));
        objs.emplace_back(
            std::make_shared<Primitive>(sphere, mat, translate(center)));
      }
    }

  // auto material1 = make_shared<dielectric>(1.5);
  //   world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  auto mat2 =
      std::make_shared<Material>(std::make_shared<SolidColor>(0.4, 0.2, 0.1));
  objs.emplace_back(
      std::make_shared<Primitive>(sphere, mat2, translate(0, 1, 0)));

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
  integrator.Render(camera, "output.ppm");
}
