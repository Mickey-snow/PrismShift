#pragma once

#include <memory>
#include <vector>

#include <shape.hpp>

class BVT;
class Primitive;
class AABB;

class Scene {
  Scene() : m_shapes{}, m_aggregator(nullptr) {}
  ~Scene() = default;

  Hit_record Hit(const Ray&, const Interval<double>&) const;
  AABB Get_Bbox(void) const;

  Scene& Add(const std::shared_ptr<IPrimitive> shape);

 private:
  std::vector<std::shared_ptr<IPrimitive>> m_shapes;
  mutable std::unique_ptr<BVT> m_aggregator;

  void Make_Aggregator(void) const;
};
