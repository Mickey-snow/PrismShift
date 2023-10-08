#ifndef SCENE_H
#define SCENE_H


#include<memory>
#include<vector>

#include<shape.hpp>
#include<aggregator.hpp>


class IAggregate;
class Primitive;
class AABB;

class PrimitiveList : IShape{
  PrimitiveList() : m_shapes{}, m_aggregator(nullptr) {}
  ~PrimitiveList() = default;

  Hit_record Hit(const Ray&, const Interval<double>&) const override;
  AABB Get_Bbox(void) const override;

  PrimitiveList& Add(const std::shared_ptr<IPrimitive> shape);

  
private:
  std::vector<std::shared_ptr<IPrimitive>> m_shapes;
  mutable std::unique_ptr<IAggregate> m_aggregator;

  void Make_Aggregator(void) const;

};

#endif
