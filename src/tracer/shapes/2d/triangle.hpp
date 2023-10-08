#ifndef TRIANGLE_H
#define TRIANGEL_H

#include<shape.hpp>

class AABB;
class Ray;
template<typename> class Interval;


class Triangle : public IShape{
public:
  Triangle() = default;
  ~Triangle() = default;

  AABB Get_Bbox() const override;
  Hit_record Hit(const Ray&, const Interval<double>&) const override;
};

#endif
