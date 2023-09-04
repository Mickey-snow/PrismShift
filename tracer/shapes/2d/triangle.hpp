#ifndef TRIANGLE_H
#define TRIANGEL_H

#include "parallelogram.hpp"
#include<util/util.hpp>
#include<factory.hpp>

#include<memory>
#include<string>
#include<sstream>


class Triangle : public Parallelogram{
public:
  static constexpr std::string name{"triangle"};
  std::string Get_Name(void) const override{ return name; }

  using Parallelogram::Parallelogram;

protected:
  virtual void Initbbox(void) override{
    bbox = AABB(AABB(Q,Q+u),AABB(Q,Q+v));
  }

  virtual bool On_Object(const double& a,const double& b) const override{
    return Parallelogram::On_Object(a,b) && (a+b)<=1;
  }
};

namespace{
  std::shared_ptr<Visible> CreateTriangle(std::stringstream& ss){
    double x,y,z;
    Vector3 Q,u,v;

    ss>>x>>y>>z; Q = Vector3(x,y,z);
    ss>>x>>y>>z; u = Vector3(x,y,z);
    ss>>x>>y>>z; v = Vector3(x,y,z);

    return std::make_shared<Triangle>(Q,u,v);
  }

  constexpr std::string Triangle_ShapeID = Triangle::name;
  const bool triangle_registered = ShapeFactory::Instance()->Register(Triangle_ShapeID, CreateTriangle);
}


#endif
