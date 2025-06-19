#pragma once

#include "camera.hpp"
#include "util/util.hpp"

#include <atomic>
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

  inline size_t GetRaycount() const noexcept { return ray_cnt_; }

 private:
  Scene& scene_;
  std::vector<std::shared_ptr<Primitive>> lights_;

  int max_depth_;
  bool mis_enabled_;

  std::atomic<size_t> ray_cnt_;
};
