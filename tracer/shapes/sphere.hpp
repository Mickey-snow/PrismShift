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

class Sphere : public Shape{
public:
  static constexpr std::string name{"sphere"};

  Sphere(const Point3& _center, const double& r):Sphere(_center,r,nullptr) {}
  Sphere(const Point3& _center, const double& r, std::shared_ptr<Material> _material) : center(_center), radius(r), material(_material) {
    double r_abs = fabs(r);
    Vector3 r_vec(r_abs,r_abs,r_abs);
    bbox = AABB(_center + r_vec, _center - r_vec);
  }

  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override;
  
  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override;
  
  std::string Get_Name(void) const override{ return name; }
  void Set_material(std::shared_ptr<Material> mat) override{ material = mat; }
private:
  Point3 center;
  double radius;
  std::shared_ptr<Material> material;

  AABB bbox;
};

namespace{
  std::shared_ptr<Shape> CreateSphere(std::stringstream& ss){
    double x,y,z;
    double r;
    ss>>x>>y>>z>>r;

    return std::make_shared<Sphere>(Vector3(x,y,z), r);
  }

  constexpr std::string Sphere_ShapeID = Sphere::name;
  const bool registered = ShapeFactory::Instance()->Register(Sphere_ShapeID, CreateSphere);
}

#endif
