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
    bbox = AABB(Q,Q+u+v).Pad();
    normal = Vector3::Unit_vector(Vector3::Cross(u,v));
    decomposer = new Decomposer3d(u,v,normal);
  }

  bool On_Object(const double& a,const double& b) const{
    return 0<=a&&a<=1 && 0<=b&&b<=1;
  }
};

namespace{
  std::shared_ptr<Visible> CreateParallelogram(Json::Value attribute){
    Json::RequireMember(attribute, "origin", "u", "v");
    Vector3 Q,u,v;
    auto qi=attribute["origin"], ui=attribute["u"], vi=attribute["v"];

    Q = Vector3(qi[0].asDouble(), qi[1].asDouble(), qi[2].asDouble());
    u = Vector3(ui[0].asDouble(), ui[1].asDouble(), ui[2].asDouble());
    v = Vector3(vi[0].asDouble(), vi[1].asDouble(), vi[2].asDouble());

    return std::make_shared<Parallelogram>(Q,u,v);
  }

  constexpr std::string Parallelogram_ShapeID = Parallelogram::name;
  const bool parallelogram_registered = ShapeFactory::Instance()->Register(Parallelogram_ShapeID, CreateParallelogram);
}


#endif
