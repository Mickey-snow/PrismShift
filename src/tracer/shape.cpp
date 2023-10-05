#include "shape.hpp"

#include<util/util.hpp>

ConcreteShape::ConcreteShape() :
  m_shape(nullptr),m_bbox_rec(nullptr) {}

ConcreteShape::~ConcreteShape() = default;

Hit_record ConcreteShape::Hit(const Ray& r, const Interval<double>& time_interval) const{
  return m_shape->Hit(r.Transform(m_frame)
		      ,time_interval);
}

AABB ConcreteShape::Get_Bbox() const{
  if(m_bbox_rec == nullptr){
    auto box = m_shape->Get_Bbox();
    m_bbox_rec = std::make_unique<AABB>(box.Transform(m_frame));
  }
  return *m_bbox_rec;
}
