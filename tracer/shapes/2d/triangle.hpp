#ifndef TRIANGLE_H
#define TRIANGEL_H

#include<util/util.hpp>
#include<factory.hpp>
#include<shape.hpp>

#include<memory>
#include<string>
#include<sstream>

#include "plane.hpp"

class Triangle : public Visible{
public:
  static constexpr std::string name{"triangle"};
  std::string Get_Name(void) const override{ return name; }

  Triangle(const Point3& _Q, const Vector3& _u, const Vector3& _v) : Q(_Q),u(_u),v(_v) { Init(); }
  static Triangle* From_points(const Point3& a,const Point3& b,const Point3& c){ return new Triangle(a,b-a,c-a); }
  static Triangle* From_pointvecvec(const Point3& a,const Vector3& u,const Vector3& v){ return new Triangle(a,u,v); }

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override;
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
    bbox = AABB(AABB(Q,Q+u),AABB(Q,Q+v)).Pad();
  }
  void Init(void){
    Initbbox();
    plane = std::make_shared<Plane>(Q,u,v);
  }

  bool On_Object(const double& a,const double& b) const{
    return (a+b)<=1 && 0<=a && 0<=b;
  }

  std::shared_ptr<Visible> Get_ptr(void) const{
    return std::make_shared<Triangle>(*this);
  }
  
};

namespace{
  std::shared_ptr<Visible> CreateTriangle(std::stringstream& ss){
    std::string method; ss>>method;
    double x,y,z;
    Point3 Q,A,B;

    ss>>x>>y>>z; Q = Vector3(x,y,z);
    ss>>x>>y>>z; A = Vector3(x,y,z);
    ss>>x>>y>>z; B = Vector3(x,y,z);

    Vector3 u,v;
    if(method == std::string{"point"}) u=A-Q, v=B-Q;
    else u=A,v=B;
    return std::make_shared<Triangle>(Q,u,v);
  }

  constexpr std::string Triangle_ShapeID = Triangle::name;
  const bool triangle_registered = ShapeFactory::Instance()->Register(Triangle_ShapeID, CreateTriangle);
}


#endif
