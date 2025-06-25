#include "aggregator.hpp"

// ---------- local helper ----------------------------------------------------
namespace {
class AxisSorter {
 public:
  explicit AxisSorter(int axis) : axis_(axis) {}
  bool operator()(const std::shared_ptr<Primitive>& a,
                  const std::shared_ptr<Primitive>& b) const {
    return AABB::Componentbased_Comparer(axis_)(a->GetBbox(), b->GetBbox());
  }

 private:
  int axis_;
};
}  // namespace

// ---------- ctor ------------------------------------------------------------
BVT::BVT(std::vector<std::shared_ptr<Primitive>> primitives)
    : primitives_(std::move(primitives)) {
  if (primitives_.empty())
    return;

  // Full binary tree
  nodes_.resize(primitives_.size() * 4);
  Build(/*nodeIdx=*/0,
        /*start   =*/0,
        /*end     =*/static_cast<uint32_t>(primitives_.size()),
        /*axis    =*/0);
}

// ---------- build -----------------------------------------------------------
void BVT::Build(uint32_t nodeIdx, uint32_t start, uint32_t end, int axis) {
  Node& node = nodes_[nodeIdx];

  // Build this node’s bounding box
  for (uint32_t i = start; i < end; ++i)
    node.bbox = (i == start) ? primitives_[i]->GetBbox()
                             : AABB(node.bbox, primitives_[i]->GetBbox());

  uint32_t span = end - start;
  if (span == 1) {  // ---- leaf ----
    node.firstPrim = start;
    node.primCount = 1;
    return;
  }

  // ---- interior ----
  uint32_t mid = start + span / 2;
  std::nth_element(primitives_.begin() + start, primitives_.begin() + mid,
                   primitives_.begin() + end, AxisSorter(axis));

  node.primCount = 0;  // mark interior

  uint32_t leftIdx = (nodeIdx << 1) + 1;
  uint32_t rightIdx = leftIdx + 1;

  Build(leftIdx, start, mid, (axis + 1) % 3);
  Build(rightIdx, mid, end, (axis + 1) % 3);
}

// ---------- traversal -------------------------------------------------------
HitRecord BVT::Hit(const Ray& r, const Interval<Float>& time) const {
  return nodes_.empty() ? HitRecord() : Traverse(0, r, time);
}

HitRecord BVT::Traverse(uint32_t nodeIdx,
                        const Ray& r,
                        Interval<Float> t) const {
  if (nodeIdx >= nodes_.size())
    return HitRecord();

  const Node& node = nodes_[nodeIdx];
  if (!node.bbox.isHitIn(r, t))
    return HitRecord();

  // ---- leaf ----
  if (node.primCount > 0) {
    HitRecord closest;
    Float bestT = t.end;
    for (uint32_t i = 0; i < node.primCount; ++i) {
      HitRecord tmp = primitives_[node.firstPrim + i]->Hit(r, t);
      if (tmp.hits && tmp.time < bestT) {
        bestT = tmp.time;
        closest = tmp;
      }
    }
    return closest;
  }

  // ---- interior ----
  uint32_t leftIdx = (nodeIdx << 1) + 1;
  uint32_t rightIdx = leftIdx + 1;

  HitRecord lh = Traverse(leftIdx, r, t);
  Float cut = lh.hits ? lh.time : t.end;
  HitRecord rh = Traverse(rightIdx, r, Interval<Float>{t.begin, cut});

  return rh.hits ? rh : lh;
}
