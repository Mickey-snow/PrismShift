#ifndef SPHERE_H
#define SPHERE_H

#include "material.hpp"
#include "visual_shape.hpp"
#include "renderer.hpp"
#include "util/util.hpp"
#include "factory.hpp"

#include<cmath>
#include<memory>
#include<sstream>
#include<string>

class Sphere : public Visible{
public:
  Sphere(const Point3& _center, const double& r):Sphere(_center,r,nullptr) {}
  Sphere(const Point3& _center, const double& r, Material* _material) : center(_center), radius(r), material(_material) {
    double r_abs = fabs(r);
    Vector3 r_vec(r_abs,r_abs,r_abs);
    bbox = AABB(_center + r_vec, _center - r_vec);
  }

  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override;
  
  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override;
  
private:
  Point3 center;
  double radius;
  Material* material;

  AABB bbox;
};

namespace{			// anonymous

  std::shared_ptr<Visible> CreateSphere(std::stringstream& ss){
    double x,y,z;
    double r;
    ss>>x>>y>>z>>r;

    return std::make_shared<Sphere>(Vector3(x,y,z), r);
  }

  constexpr std::string Sphere_ShapeID("sphere");

  const bool registered = ShapeFactory::Instance()->RegisterVisible(Sphere_ShapeID, CreateSphere);
  
}

#endif
