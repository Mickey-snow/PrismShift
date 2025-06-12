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
  double vFovDeg = 23;
  // Camera(center, lookAt, imageHeight, aspect, verticalFovDegrees)
  Camera cam({13, 2, -3}, {0, 0, 0}, image_h, aspect, vFovDeg);
  return cam;
}

inline static std::shared_ptr<Primitive>
make_sphere(std::shared_ptr<Material> mat, Point3 pos, double d) {
  static const auto sphere = std::make_shared<Sphere>();

  auto trans = std::make_shared<CompositeTransformation>(
      std::make_shared<VectorScale>(d, d, d),
      std::make_shared<VectorTranslate>(Vector3(pos)));
  // Note: transformation order is equivalent to
  // MatrixTransformation::Translate(Vector3(pos)) *
  //     MatrixTransformation::Scale(d, d, d)

  return std::make_shared<Primitive>(sphere, std::move(mat), std::move(trans));
}
inline static std::shared_ptr<Primitive> make_sphere(Color color,
                                                     Point3 pos,
                                                     double d) {
  auto mat = std::make_shared<Material>(
      std::make_shared<SolidColor>(std::move(color)));
  return make_sphere(std::move(mat), std::move(pos), d);
}

Scene Get_World() {
  std::vector<std::shared_ptr<Primitive>> objs;

  // ground
  objs.emplace_back(
      make_sphere(Color(0.5, 0.5, 0.5), Point3(0, -1000, 0), 1000));
  for (int a = -11; a < 11; ++a)
    for (int b = -11; b < 11; ++b) {
      Point3 center(a + random_double(0, 0.9), 0.2, b + random_double(0, 0.9));

      if ((center - Point3(4, 1, 1)).Length() > 1.2) {
        auto albedo = Color(random_double(0.65, 1), random_double(0.65, 1),
                            random_double(0.65, 1));
        objs.emplace_back(make_sphere(albedo, center, 0.2));
      }
    }

  objs.emplace_back(make_sphere(Color(0.4, 0.1, 0.9), Point3(-4, 1, 1), 1));
  objs.emplace_back(make_sphere(
      std::make_shared<Material>(nullptr, std::make_shared<FloatConst>(1.5)),
      Point3(0, 1, 1), 1));
  // objs.emplace_back(
  //     make_sphere(std::make_shared<Material>(
  //                     std::make_shared<SolidColor>(Color(0.7, 0.6, 0.5)),
  //                     std::make_shared<FloatConst>(0.03)),
  //                 Point3(4, 1, 1), 1));

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
  Integrator integrator(scene, 50);
  integrator.Render(camera, "output.ppm", 32);
}
