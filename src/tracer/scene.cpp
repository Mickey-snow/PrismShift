#include "scene.hpp"

#include<primitive.hpp>
#include<util/util.hpp>
#include<bvh.hpp>

PrimitiveList& PrimitiveList::Add(const std::shared_ptr<Primitive> shape){
  m_shapes.push_back(shape);
  m_aggregator = nullptr;
  return *this;
}

Hit_record PrimitiveList::Hit(const Ray& r, const Interval<double>& time) const {
  Make_Aggregator();
  return m_aggregator->Hit(r,time);
}

AABB PrimitiveList::Get_Bbox(void) const{
  Make_Aggregator();
  return m_aggregator->Get_Bbox();
}

using Aggregator = BVT;
void PrimitiveList::Make_Aggregator(void) const{
  if(m_aggregator == nullptr)
    m_aggregator = std::make_unique<Aggregator>(m_shapes);
}
