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

class Plane : public Visible{
public:
  static constexpr std::string name{"plane"};
  virtual std::string Get_Name(void) const override{ return name; }

  Plane(Point3 _Q, Vector3 _u, Vector3 _v) : Q(_Q),v(_v),u(_u){ Init(); }

  void Set_material(std::shared_ptr<Material> mat) override{ material = mat; }

  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override;

  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override;

  virtual Point2 Map_Texture(const Ray&, const Hit_record&) const override;

protected:
  AABB bbox;
  Point3 Q;
  Vector3 u,v;

  std::shared_ptr<Material> material;
  
  Vector3 normal;
  double dist_to_orig;
  Vector3 Omega;
  virtual void InitConstant(void){
    normal = Vector3::Unit_vector(Vector3::Cross(u,v));
    dist_to_orig = Vector3::Dot(normal, Q);
    Omega = normal / (Vector3::Dot(normal, Vector3::Cross(u,v)));

  }
  virtual void Initbbox(void){
    bbox = AABB(Interval<double>::Universe(), Interval<double>::Universe(), Interval<double>::Universe());
  }    
  virtual void Init(void){
    InitConstant();
    Initbbox();
  }

  virtual std::pair<bool, Point3> Intersection(const Ray& r, const Interval<double>& time) const;
  virtual std::pair<double,double> Decomposition(const Vector3&) const;

  virtual bool On_Object(const double&,const double&) const;
};

namespace{
  std::shared_ptr<Visible> CreatePlane(std::stringstream& ss){
    double x,y,z;
    Vector3 Q,u,v;

    ss>>x>>y>>z; Q = Vector3(x,y,z);
    ss>>x>>y>>z; u = Vector3(x,y,z);
    ss>>x>>y>>z; v = Vector3(x,y,z);

    return std::make_shared<Plane>(Q,u,v);
  }

  constexpr std::string Plane_ShapeID = Plane::name;
  const bool plane_registered = ShapeFactory::Instance()->Register(Plane_ShapeID, CreatePlane);
}

#endif
