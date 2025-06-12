#pragma once

#include "camera.hpp"
#include "util/util.hpp"

#include <string>

class Scene;

class Integrator {
 public:
  Integrator(Scene& scene, int max_depth)
      : scene_(scene), max_depth_(max_depth) {}

  Color Li(Ray ray, int depth = 0);

  void Render(const Camera& cam, std::string output_filename, const int spp=32);

 private:
  Scene& scene_;
  int max_depth_;
};
