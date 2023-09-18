#ifndef SPHERE_H
#define SPHERE_H

#include<material.hpp>
#include<shape.hpp>
#include<util/util.hpp>
#include<factory.hpp>

#include<cmath>
#include<memory>
#include<sstream>
#include<string>

class Sphere : public Visible{
public:
  static constexpr std::string name{"sphere"};

  Sphere(const Point3& _center, const double& r):Sphere(_center,r,nullptr) {}
  Sphere(const Point3& _center, const double& r, std::shared_ptr<Material> _material):
    Visible(Coordinate3().Set_Translation(Coordinate3::Origin(_center)).Set_Scale(r,r,r)),
    center(_center), radius(r), material(_material) {
    double r_abs = fabs(r);
    Vector3 r_vec(r_abs,r_abs,r_abs);
    bbox = AABB(_center + r_vec, _center - r_vec);
  }

  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override;

  virtual Point2 Map_Texture(const Hit_record& rec) const override;

  
  std::string Get_Name(void) const override{ return name; }
  void Set_Material(std::shared_ptr<Material> mat) override{ material = mat; }
  std::shared_ptr<Material> Get_Material(void) override{ return material; }
  
private:
  Point3 center;
  double radius;
  std::shared_ptr<Material> material;

  AABB bbox;
};

namespace{
  std::shared_ptr<Visible> CreateSphere(Json::Value attribute){
    Json::RequireMember(attribute, "r", "center");
    double r = attribute["r"].asDouble();
    double x = attribute["center"][0].asDouble();
    double y= attribute["center"][1].asDouble();
    double z = attribute["center"][2].asDouble();

    return std::make_shared<Sphere>(Point3(x,y,z), r);
  }

  constexpr std::string Sphere_ShapeID = Sphere::name;
  const bool sphere_registered = ShapeFactory::Instance()->Register(Sphere_ShapeID, CreateSphere);
}

#endif
