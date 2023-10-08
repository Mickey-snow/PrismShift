#include "primitive.hpp"
#include<shape.hpp>
#include<material.hpp>
#include<bsdf.hpp>
#include<util/util.hpp>


Primitive::Primitive() :
  m_material(nullptr), m_shape(nullptr) {}
Primitive::~Primitive() = default;


Hit_record Primitive::Hit(const Ray& r, const Interval<double>& t) const{
  if(m_shape == nullptr) return Hit_record{};
  else return m_shape->Hit(r,t);
}

AABB Primitive::Get_Bbox(void) const{
  if(m_shape == nullptr) return AABB();
  else return m_shape->Get_Bbox();
}

BSDF Primitive::CalcBSDF(const Hit_record& rec) const{
  if(m_material == nullptr) return {};
  else return m_material->CalcBSDF(rec);
}
