#ifndef SPHERE_H
#define SPHERE_H

#include "objects.hpp"
#include "common/common.hpp"

#include<cmath>

class Sphere : public Visible{
public:
  Sphere(const Point3& _center, const double& r) : center(_center), radius(r) {}

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override{
    return 0.5*(rec.normal+Color(1,1,1));
  }
  
  virtual Hit_record Ray_Hit(const Ray& r, const Interval& time) const override{
    Hit_record rec; rec.hits = false;
    
    Vector3 oc = r.Origin() - center;
    auto a = r.Direction().Length_squared();
    auto half_b = Dot(oc, r.Direction());
    auto c = oc.Length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return rec;
    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (!time.Surrounds(root)){
      root = (-half_b + sqrtd) / a;
      if (!time.Surrounds(root))
	return rec;
    }

    rec.hits = true;
    rec.hitted_obj = std::make_shared<Sphere>(*this);
    
    rec.time = root;
    rec.position = r.At(rec.time);
    Vector3 outward_normal = (rec.position - center) / radius;
    rec.Set_Face_Normal(r, outward_normal);
    
    return rec;
  }
  
private:
  Point3 center;
  double radius;
};

#endif
