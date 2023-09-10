#ifndef PLANE_H
#define PLANE_H

#include<util/util.hpp>
#include<shape.hpp>
#include<material.hpp>
#include<factory.hpp>

#include<utility>
#include<memory>
#include<string>
#include<sstream>

class Parallelogram;
class Triangle;
class Plane : public Visible{
public:
  static constexpr std::string name{"plane"};
  virtual std::string Get_Name(void) const override{ return name; }

  Plane(Point3 _Q, Vector3 _u, Vector3 _v) : Q(_Q),v(_v),u(_u){ Init(); }

  void Set_Material(std::shared_ptr<Material> mat) override{ material = mat; }
  std::shared_ptr<Material> Get_Material(void) override{ return material; }
  
  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override;

  virtual Point2 Map_Texture(const Hit_record&) const override;

protected:
  AABB bbox;
  Point3 Q;
  Vector3 u,v,normal;

  std::shared_ptr<Material> material;
  Decomposer3d *decomposer;

  virtual void Init(void){
    normal = Vector3::Unit_vector(Vector3::Cross(u,v));
    bbox = AABB(Interval<double>::Universe(), Interval<double>::Universe(), Interval<double>::Universe());
    decomposer = new Decomposer3d(u,v,normal);
  }
};

namespace{
  std::shared_ptr<Visible> CreatePlane(Json::Value attribute){
    Json::RequireMember(attribute, "origin", "u", "v");
    Vector3 Q,u,v;
    auto qi=attribute["origin"], ui=attribute["u"], vi=attribute["v"];

    Q = Vector3(qi[0].asDouble(), qi[1].asDouble(), qi[2].asDouble());
    u =Vector3(ui[0].asDouble(), ui[1].asDouble(), ui[2].asDouble());
    v = Vector3(vi[0].asDouble(), vi[1].asDouble(), vi[2].asDouble());

    return std::make_shared<Plane>(Q,u,v);
  }

  constexpr std::string Plane_ShapeID = Plane::name;
  const bool plane_registered = ShapeFactory::Instance()->Register(Plane_ShapeID, CreatePlane);
}

#endif
