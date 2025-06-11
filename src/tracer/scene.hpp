#pragma once

#include "aggregator.hpp"
#include "shape.hpp"
#include "util/util.hpp"

#include <functional>
#include <memory>
#include <vector>

class BVT;
class AABB;

class Scene {
 public:
  Scene(std::vector<std::shared_ptr<Primitive>> objs);
  ~Scene() = default;

  Scene(Scene&&) noexcept = default;
  Scene& operator=(Scene&&) noexcept = default;

  HitRecord Hit(Ray ray, Interval<double> interval) const;
  AABB GetBbox(void) const;
  void SetBackground(std::function<Color(Ray)> fn);
  Color Background(Ray ray) const;

  std::vector<std::shared_ptr<Primitive>> objs_;
  std::unique_ptr<BVT> aggregator_;
  std::function<Color(Ray)> background_fn_;
};
