#include "aggregator.hpp"

#include <primitive.hpp>
#include <shape.hpp>
#include <util/util.hpp>

#include <algorithm>
#include <format>
#include <memory>
#include <stdexcept>

BVT::BVT(const std::vector<std::shared_ptr<Primitive>>& li) {
  root = std::make_unique<Node>(li);
  bbox = root->GetBbox();
}

HitRecord BVT::Hit(const Ray& r, const Interval<Float>& time_interval) const {
  return root->Hit(r, time_interval);
}

// helpers
inline static AABB _GetBbox(const BVT::Node::var_t& var) {
  return std::visit(
      overload([](std::shared_ptr<BVT::Node> node) { return node->bbox; },
               [](std::shared_ptr<Primitive> prim) { return prim->GetBbox(); }),
      var);
}
inline static HitRecord _GetHitResult(const BVT::Node::var_t& var,
                                      Ray r,
                                      Interval<Float> t) {
  return std::visit(
      overload(
          [&](std::shared_ptr<BVT::Node> node) { return node->Hit(r, t); },
          [&](std::shared_ptr<Primitive> prim) { return prim->Hit(r, t); }),
      var);
}

BVT::Node::Node(std::vector<std::shared_ptr<Primitive>>& src_obj,
                size_t start,
                size_t end,
                int axis) {
  class AxisComparer {
   public:
    AxisComparer(const AABB::Componentbased_Comparer& cp)
        : component_comparer(cp) {}
    bool operator()(std::shared_ptr<Primitive> a,
                    std::shared_ptr<Primitive> b) {
      return component_comparer(a->GetBbox(), b->GetBbox());
    }

   private:
    AABB::Componentbased_Comparer component_comparer;
  };

  auto comparer = AxisComparer(AABB::Componentbased_Comparer(axis));
  size_t object_span = end - start;

  if (object_span == 1)
    lch = rch = src_obj[start];
  else if (object_span == 2) {
    if (comparer(src_obj[start], src_obj[start + 1])) {
      lch = src_obj[start];
      rch = src_obj[start + 1];
    } else {
      lch = src_obj[start + 1];
      rch = src_obj[start];
    }
  } else if (object_span >= 3) {
    size_t mid = start + object_span / 2;
    std::nth_element(src_obj.begin() + start, src_obj.begin() + mid,
                     src_obj.begin() + end, comparer);
    int next_axis = (axis + 1) % 3;

    lch = std::make_shared<Node>(src_obj, start, mid, next_axis);
    rch = std::make_shared<Node>(src_obj, mid, end, next_axis);
  } else
    throw std::runtime_error(std::format(
        "Array offset error while constructing bvt. start={}, end={}, span={}",
        start, end, object_span));

  bbox = AABB{_GetBbox(lch), _GetBbox(rch)};
}

HitRecord BVT::Node::Hit(const Ray& r, const Interval<Float>& time) const {
  Float closest_hit_time = time.end;

  if (!bbox.isHitIn(r, time))
    return HitRecord();
  HitRecord rec = HitRecord();

  HitRecord temp_rec = _GetHitResult(lch, r, time);
  if (temp_rec.hits) {
    rec = temp_rec;
    closest_hit_time = temp_rec.time;
  }

  temp_rec = _GetHitResult(rch, r, Interval{time.begin, closest_hit_time});
  if (temp_rec.hits)
    rec = temp_rec;

  return rec;
}
