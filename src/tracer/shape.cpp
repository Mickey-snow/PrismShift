#include "shape.hpp"

#include <util/util.hpp>

ConcreteShape::ConcreteShape() : m_shape(nullptr) {}

ConcreteShape::~ConcreteShape() {}

HitRecord ConcreteShape::Hit(const Ray& r,
                              const Interval<double>& time_interval) const {
  if (!m_shape)
    throw std::logic_error("shape pointer is null");

  return m_shape->Hit(r.Transform(m_frame), time_interval);
}

AABB ConcreteShape::GetBbox() const {
  if (!m_bbox_rec && m_shape) {
    auto box = m_shape->GetBbox();
    m_bbox_rec = box.Transform(m_frame);
  }
  return m_bbox_rec.value_or(AABB{});
}
