#pragma once

#include "simple_renderer.hpp"

inline Camera Get_Camera() {
  int image_h = 720;
  double aspect = 16.0 / 9.0;
  double vFovDeg = 60.0;
  // Camera(center, lookAt, imageHeight, aspect, verticalFovDegrees)
  Camera cam({0, 0, 0}, {0, 0, -1}, image_h, aspect, vFovDeg);
  return cam;
}

inline Scene Get_World() {
  Scene scene;
  scene.objects.push_back(Sphere{Point3(0, 0, -1), 0.5, Color(0.7, 0.3, 0.3)});
  scene.objects.push_back(
      Sphere{Point3(0, -100.5, -1), 100, Color(0.8, 0.8, 0.0)});
  return scene;
}
