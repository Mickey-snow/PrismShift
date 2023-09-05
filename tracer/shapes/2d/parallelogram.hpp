#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include<util/util.hpp>
#include<shape.hpp>
#include<factory.hpp>

#include<memory>
#include<string>
#include<sstream>

#include "plane.hpp"

class Parallelogram : public Visible{
public:
  static constexpr std::string name{"parallelogram"};
  std::string Get_Name(void) const override{ return name; }
  
  Parallelogram(const Point3& _Q, const Vector3& _u, const Vector3& _v) : Q(_Q),u(_u),v(_v) { Init(); }
  
  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override;
  virtual Hit_record Ray_Hit(const Ray&, const Interval<double>&) const override;
  Point2 Map_Texture(const Ray&, const Hit_record&) const override;

  AABB Get_Bounding_box(void) const override{ return bbox; }
  void Set_material(std::shared_ptr<Material> mat) override{ material = mat; }
  

protected:
  AABB bbox;
  Point3 Q;
  Vector3 u,v;

  std::shared_ptr<Material> material;
  std::shared_ptr<Plane> plane;
  
  void Initbbox(void){
    bbox = AABB(Q,Q+u+v).Pad();
  }
  void Init(void){
    Initbbox();
    plane = std::make_shared<Plane>(Q,u,v);
  }

  bool On_Object(const double& a,const double& b) const{
    return 0<=a&&a<=1 && 0<=b&&b<=1;
  }

  std::shared_ptr<Visible> Get_ptr(void) const{
    return std::make_shared<Parallelogram>(*this);
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
