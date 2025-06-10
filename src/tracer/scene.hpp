#pragma once

#include "shape.hpp"
#include "util/util.hpp"

#include <functional>
#include <memory>
#include <vector>

class BVT;
class IPrimitive;
class AABB;

class Scene {
 public:
  Scene(std::vector<std::shared_ptr<Primitive>> objs);
  ~Scene();

  HitRecord Hit(Ray ray, Interval<double> interval) const;
  AABB GetBbox(void) const;
  void SetBackground(std::function<Color(Ray)> fn);
  Color Background(Ray ray) const;

 private:
  std::vector<std::shared_ptr<Primitive>> objs_;
  std::unique_ptr<BVT> aggregator_;
  std::function<Color(Ray)> background_fn_;
};
