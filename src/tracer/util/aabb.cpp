#include "aabb.hpp"

#include <algorithm>
#include <format>
#include <functional>
#include <initializer_list>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <vector>

#include "ray.hpp"
#include "util/transform.hpp"
#include "util/vector.hpp"

AABB::AABB(std::initializer_list<Point3> li) {
  if (li.size() == 0) {
    x_interval = y_interval = z_interval = Interval<Float>::Empty;
    return;
  }

  auto get_x_comp = [](const Point3& p) { return p.x(); };
  auto get_y_comp = [](const Point3& p) { return p.y(); };
  auto get_z_comp = [](const Point3& p) { return p.z(); };

  auto max_x = std::ranges::max(li | std::views::transform(get_x_comp));
  auto min_x = std::ranges::min(li | std::views::transform(get_x_comp));
  auto max_y = std::ranges::max(li | std::views::transform(get_y_comp));
  auto min_y = std::ranges::min(li | std::views::transform(get_y_comp));
  auto max_z = std::ranges::max(li | std::views::transform(get_z_comp));
  auto min_z = std::ranges::min(li | std::views::transform(get_z_comp));

  x_interval = Interval<Float>(min_x, max_x);
  y_interval = Interval<Float>(min_y, max_y);
  z_interval = Interval<Float>(min_z, max_z);
}

AABB::AABB(std::initializer_list<AABB> li) {
  if (li.size() == 0) {
    x_interval = y_interval = z_interval = Interval<Float>::Empty;
    return;
  }

  auto get_axis = [](const AABB& box, const int axis) {
    return box.Axis(axis);
  };
  auto get_min = [](const Interval<Float>& it) { return it.begin; };
  auto get_max = [](const Interval<Float>& it) { return it.end; };

  using namespace std::placeholders;
  auto get_x_axis = std::bind(get_axis, _1, 0);
  auto max_x = std::ranges::max(li | std::views::transform(get_x_axis) |
                                std::views::transform(get_max));
  auto min_x = std::ranges::min(li | std::views::transform(get_x_axis) |
                                std::views::transform(get_min));
  auto get_y_axis = std::bind(get_axis, _1, 1);
  auto max_y = std::ranges::max(li | std::views::transform(get_y_axis) |
                                std::views::transform(get_max));
  auto min_y = std::ranges::min(li | std::views::transform(get_y_axis) |
                                std::views::transform(get_min));
  auto get_z_axis = std::bind(get_axis, _1, 2);
  auto max_z = std::ranges::max(li | std::views::transform(get_z_axis) |
                                std::views::transform(get_max));
  auto min_z = std::ranges::min(li | std::views::transform(get_z_axis) |
                                std::views::transform(get_min));

  x_interval = Interval<Float>(min_x, max_x);
  y_interval = Interval<Float>(min_y, max_y);
  z_interval = Interval<Float>(min_z, max_z);
}
AABB::AABB(const Point3& a, const Point3& b) {
  x_interval = Interval<Float>(fmin(a.x(), b.x()), fmax(a.x(), b.x()));
  y_interval = Interval<Float>(fmin(a.y(), b.y()), fmax(a.y(), b.y()));
  z_interval = Interval<Float>(fmin(a.z(), b.z()), fmax(a.z(), b.z()));
}
AABB::AABB(const AABB& a, const AABB& b)
    : x_interval(Interval(a.x_interval, b.x_interval)),
      y_interval(Interval(a.y_interval, b.y_interval)),
      z_interval(Interval(a.z_interval, b.z_interval)) {}

const Interval<Float>& AABB::Axis(const int& n) const {
  if (n == 0)
    return x_interval;
  if (n == 1)
    return y_interval;
  if (n == 2)
    return z_interval;
  throw std::range_error(
      std::format("at AABB::Axis: accepts n=0,1,2 for x,y,z interval "
                  "components. given n={}",
                  n));
}

bool AABB::isHitIn(const Ray& r, Interval<Float> ray_t) const {
  for (int a = 0; a < 3; ++a) {
    auto invD = 1.0 / r.Direction()[a];
    auto orig = r.Origin()[a];

    auto t0 = (Axis(a).begin - orig) * invD;
    auto t1 = (Axis(a).end - orig) * invD;

    if (invD < 0)
      std::swap(t0, t1);

    // Overlap the (a) component's interval [t0,t1]
    // Let ray_t be the new effective time interval
    if (t0 > ray_t.begin)
      ray_t.begin = t0;
    if (t1 < ray_t.end)
      ray_t.end = t1;

    if (ray_t.end <= ray_t.begin)
      return false;
  }
  return true;
}
bool AABB::Contains(const Point3& p) const {
  const Float x = p.x(), y = p.y(), z = p.z();
  return x_interval.Contains(x) && y_interval.Contains(y) &&
         z_interval.Contains(z);
}
bool AABB::Contains(const AABB& box) const {
  for (int i = 0; i < 3; ++i)
    if (!box.Axis(i).In(Axis(i)))
      return false;
  return true;
}

bool AABB::isEmpty(void) const {
  return x_interval.isEmpty() && y_interval.isEmpty() && z_interval.isEmpty();
}

AABB AABB::Pad() const {
  static Float EPS = 1e-3;
  Interval<Float> xx =
      (x_interval.Size() < EPS) ? x_interval.Expand(EPS) : x_interval;
  Interval<Float> yy =
      (y_interval.Size() < EPS) ? y_interval.Expand(EPS) : y_interval;
  Interval<Float> zz =
      (z_interval.Size() < EPS) ? z_interval.Expand(EPS) : z_interval;
  return AABB(xx, yy, zz);
}

AABB AABB::Transform(const ITransformation& tr) const {
  std::array<Point3, 8> v{
      Point3(x_interval.begin, y_interval.begin, z_interval.begin),
      Point3(x_interval.begin, y_interval.begin, z_interval.end),
      Point3(x_interval.begin, y_interval.end, z_interval.begin),
      Point3(x_interval.begin, y_interval.end, z_interval.end),
      Point3(x_interval.end, y_interval.begin, z_interval.begin),
      Point3(x_interval.end, y_interval.begin, z_interval.end),
      Point3(x_interval.end, y_interval.end, z_interval.begin),
      Point3(x_interval.end, y_interval.end, z_interval.end)};
  for (auto& it : v)
    it = tr.Doit(it);

  return AABB{v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]};
}

AABB AABB::UndoTransform(const ITransformation& tr) const {
  std::array<Point3, 8> v{
      Point3(x_interval.begin, y_interval.begin, z_interval.begin),
      Point3(x_interval.begin, y_interval.begin, z_interval.end),
      Point3(x_interval.begin, y_interval.end, z_interval.begin),
      Point3(x_interval.begin, y_interval.end, z_interval.end),
      Point3(x_interval.end, y_interval.begin, z_interval.begin),
      Point3(x_interval.end, y_interval.begin, z_interval.end),
      Point3(x_interval.end, y_interval.end, z_interval.begin),
      Point3(x_interval.end, y_interval.end, z_interval.end)};
  for (auto& it : v)
    it = tr.Undo(it);

  return AABB{v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]};
}
