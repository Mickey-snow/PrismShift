#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include<util/util.hpp>
#include<shape.hpp>
#include<factory.hpp>

#include<memory>
#include<string>
#include<sstream>

#include "plane.hpp"

class Parallelogram : public Plane{
public:
  static constexpr std::string name{"parallelogram"};
  std::string Get_Name(void) const override{ return name; }
  
  using Plane::Plane;

protected:
  virtual void Initbbox(void) override{
    bbox = AABB(Q,Q+u+v);
  }

  virtual bool On_Object(const double& a,const double& b) const override{
    return 0<=a&&a<=1 && 0<=b&&b<=1;
  }
};

namespace{
  std::shared_ptr<Visible> CreateParallelogram(std::stringstream& ss){
    double x,y,z;
    Vector3 Q,u,v;

    ss>>x>>y>>z; Q = Vector3(x,y,z);
    ss>>x>>y>>z; u = Vector3(x,y,z);
    ss>>x>>y>>z; v = Vector3(x,y,z);

    return std::make_shared<Parallelogram>(Q,u,v);
  }

  constexpr std::string Parallelogram_ShapeID = Parallelogram::name;
  const bool parallelogram_registered = ShapeFactory::Instance()->Register(Parallelogram_ShapeID, CreateParallelogram);
}


#endif
