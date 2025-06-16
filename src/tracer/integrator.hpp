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
  Integrator(Scene& scene, int max_depth);

  Color Li(Ray ray, int depth = 0, bool use_mis = true);

  void Render(const Camera& cam,
              std::string output_filename,
              const int spp = 32);

 private:
  Scene& scene_;
  int max_depth_;
  std::vector<std::shared_ptr<Primitive>> lights_;
};
