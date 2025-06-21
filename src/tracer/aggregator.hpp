#pragma once

#include <primitive.hpp>
#include <shape.hpp>
#include <util/util.hpp>

#include <memory>
#include <string>
#include <variant>
#include <vector>

// FIXME: Fail to construct when primitive array is empty
class BVT {
 public:
  struct Node {
    Node(const std::vector<std::shared_ptr<Primitive>>& src) {
      std::vector<std::shared_ptr<Primitive>> src_obj_copy(src);
      *this = Node(src_obj_copy, 0, src_obj_copy.size(), 0);
    }
    Node(std::vector<std::shared_ptr<Primitive>>& src_obj,
         size_t start,
         size_t end,
         int axis = 0);

    HitRecord Hit(const Ray&, const Interval<double>&) const;
    AABB GetBbox(void) const noexcept { return bbox; }

    using var_t =
        std::variant<std::shared_ptr<Node>, std::shared_ptr<Primitive>>;
    var_t lch, rch;
    AABB bbox;
  };

 public:
  BVT(const std::vector<std::shared_ptr<Primitive>>& li);

  AABB GetBbox(void) const { return bbox; }
  HitRecord Hit(const Ray&, const Interval<double>&) const;

 private:
  AABB bbox;
  std::unique_ptr<Node> root;
};
