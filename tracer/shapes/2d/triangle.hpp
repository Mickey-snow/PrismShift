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

  virtual Color Ray_Color(const Hit_record& rec) const override;
  virtual Hit_record Ray_Hit(const Ray&, const Interval<double>&) const override;
  Point2 Map_Texture(const Hit_record&) const override;

  AABB Get_Bounding_box(void) const override{ return bbox; }
  void Set_material(std::shared_ptr<Material> mat) override{ material = mat; }

protected:
  AABB bbox;
  Point3 Q;
  Vector3 u,v,normal;

  std::shared_ptr<Material> material;
  Decomposer3d* decomposer;
  
  void Init(void){
    normal = Vector3::Unit_vector(Vector3::Cross(u,v));
    bbox = AABB(AABB(Q,Q+u),AABB(Q,Q+v)).Pad();
    decomposer = new Decomposer3d(u,v,normal);
  }

  bool On_Object(const double& a,const double& b) const{
    return (a+b)<=1 && 0<=a && 0<=b;
  }
};

namespace{
  std::shared_ptr<Visible> CreateTriangle(Json::Value attribute){
    Json::RequireMember(attribute, "w", "u", "v");
    Vector3 u,v,w;
    auto wi=attribute["w"], ui=attribute["u"], vi=attribute["v"];

    u =Vector3(ui[0].asDouble(), ui[1].asDouble(), ui[2].asDouble());
    v = Vector3(vi[0].asDouble(), vi[1].asDouble(), vi[2].asDouble());
    w = Vector3(wi[0].asDouble(), wi[1].asDouble(), wi[2].asDouble());

    return std::shared_ptr<Triangle>(Triangle::From_points(w, u, v));
  }

  constexpr std::string Triangle_ShapeID = Triangle::name;
  const bool triangle_registered = ShapeFactory::Instance()->Register(Triangle_ShapeID, CreateTriangle);
}


#endif
