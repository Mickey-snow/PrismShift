#include "primitive.hpp"
#include <bsdf.hpp>
#include <material.hpp>
#include <shape.hpp>
#include <util/util.hpp>

Primitive::Primitive() : m_material(nullptr), m_shape(nullptr) {}
Primitive::~Primitive() = default;

Hit_record Primitive::Hit(const Ray& r, const Interval<double>& t) const {
  if (m_shape == nullptr)
    return Hit_record{};

  auto rec = m_shape->Hit(r, t);
  if (rec.isHit())
    rec.hitted_obj = this;
  return rec;
}

AABB Primitive::Get_Bbox(void) const {
  if (m_shape == nullptr)
    return AABB();
  else
    return m_shape->Get_Bbox();
}

BSDF Primitive::CalcBSDF(const Hit_record& rec) const {
  if (m_material == nullptr)
    return {};
  else
    return m_material->CalcBSDF(rec);
}
