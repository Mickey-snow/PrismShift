#include "scene.hpp"

#include <aggregator.hpp>
#include <primitive.hpp>
#include <util/util.hpp>

Scene& Scene::Add(const std::shared_ptr<IPrimitive> shape) {
  m_shapes.push_back(shape);
  m_aggregator = nullptr;
  return *this;
}

Hit_record Scene::Hit(const Ray& r, const Interval<double>& time) const {
  Make_Aggregator();
  return m_aggregator->Hit(r, time);
}

AABB Scene::Get_Bbox(void) const {
  Make_Aggregator();
  return m_aggregator->Get_Bbox();
}

void Scene::Make_Aggregator(void) const {
  if (m_aggregator == nullptr)
    m_aggregator = std::make_unique<BVT>(m_shapes);
}
