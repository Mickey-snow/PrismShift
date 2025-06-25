#pragma once

#include "aggregator.hpp"
#include "shape.hpp"
#include "util/util.hpp"

#include <functional>
#include <memory>
#include <span>
#include <vector>

class BVT;
class AABB;

class Scene {
 public:
  Scene(std::vector<std::shared_ptr<Primitive>> objs);
  ~Scene() = default;

  Scene(Scene&&) noexcept = default;
  Scene& operator=(Scene&&) noexcept = default;

  HitRecord Hit(Ray ray, Interval<Float> interval) const;
  AABB GetBbox(void) const;
  void SetBackground(std::function<Color(Ray)> fn);
  Color Background(Ray ray) const;

  inline std::span<const std::shared_ptr<Primitive>> GetPrimitives() const {
    return aggregator_->GetPrimitives();
  }

 private:
  std::unique_ptr<BVT> aggregator_;
  std::function<Color(Ray)> background_fn_;
};
