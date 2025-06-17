#pragma once

#include "camera.hpp"
#include "util/util.hpp"

#include <memory>
#include <string>
#include <vector>

class Scene;
class Primitive;

class Integrator {
 public:
  Integrator(Scene& scene, int max_depth, bool mis_enabled = true);

  Color Li(Ray ray, int depth = 0);

  void Render(const Camera& cam, std::string output_filename, const int spp);

 private:
  Scene& scene_;
  int max_depth_;
  bool mis_enabled_;
  std::vector<std::shared_ptr<Primitive>> lights_;
};
