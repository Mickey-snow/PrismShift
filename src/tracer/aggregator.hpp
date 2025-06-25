#pragma once

#include "primitive.hpp"
#include "shape.hpp"
#include "util/util.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

class BVT {
 public:
  explicit BVT(std::vector<std::shared_ptr<Primitive>> primitives);

  HitRecord Hit(const Ray& r, const Interval<Float>& time) const;
  const AABB& GetBbox() const noexcept { return nodes_.front().bbox; }

  inline std::span<const std::shared_ptr<Primitive>> GetPrimitives() const {
    return primitives_;
  }

 private:
  struct Node {
    AABB bbox;           // bounds of this node
    uint32_t firstPrim;  // offset in primitives_ (valid when primCount>0)
    uint32_t primCount;  // 0 -> interior, >0 -> leaf
  };

  // recursive build / traversal helpers
  void Build(uint32_t nodeIdx, uint32_t start, uint32_t end, int axis);

  HitRecord Traverse(uint32_t nodeIdx, const Ray& r, Interval<Float> t) const;

  std::vector<Node> nodes_;  // implicit binary tree
  std::vector<std::shared_ptr<Primitive>> primitives_;
};
