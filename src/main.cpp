#include "integrator.hpp"
#include "scene.hpp"
#include "shape.hpp"

Camera Get_Camera() {
  int image_h = 720;
  double aspect = 16.0 / 9.0;
  double vFovDeg = 60.0;
  // Camera(center, lookAt, imageHeight, aspect, verticalFovDegrees)
  Camera cam({0, 0, 0}, {0, 0, -1}, image_h, aspect, vFovDeg);
  return cam;
}

Scene Get_World() {
  std::vector<std::shared_ptr<Primitive>> objs;

  //   Scene scene;
  //   scene.objects.push_back(Sphere{Point3(0, 0, -1), 0.5, Color(0.7, 0.3,
  //   0.3)}); scene.objects.push_back(
  //       Sphere{Point3(0, -100.5, -1), 100, Color(0.8, 0.8, 0.0)});
  //   return scene;
  return Scene(std::move(objs));
}

int main() {
  auto camera = Get_Camera();
  auto scene = Get_World();
  Integrator integrator(scene, 32);
  integrator.Render(camera, "output.ppm");
}
