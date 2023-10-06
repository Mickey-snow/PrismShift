#include "primitive.hpp"
#include<shape.hpp>
#include<material.hpp>


Primitive::Primitive() :
  m_material(nullptr), m_shape(nullptr) {}
Primitive::~Primitive() = default;


Hit_record Primitive::Hit(const Ray& r, const Interval<double>& t) const{
  if(m_shape == nullptr) return Hit_record::NoHit();
  else return m_shape->Hit(r,t);
}


BSDF Primitive::CalcBSDF(const Hit_record& rec) const{
  if(m_material == nullptr) return {};
  else return m_material->CalcBSDF(rec);
}
