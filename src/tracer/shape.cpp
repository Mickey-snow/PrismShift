#include "shape.hpp"

#include<util/util.hpp>

ConcreteShape::ConcreteShape() :
  m_shape(nullptr) {}

ConcreteShape::~ConcreteShape() = default;

Hit_record ConcreteShape::Hit(const Ray& r, const Interval<double>& time_interval) const{
  return m_shape->Hit(r,time_interval);
}

AABB ConcreteShape::Get_Bbox() const{
  if(m_bbox_rec == nullptr){
    auto box = m_shape->Get_Bbox();
    m_bbox_rec = std::make_unique<AABB>(box);
  }
  return *m_bbox_rec;
}
