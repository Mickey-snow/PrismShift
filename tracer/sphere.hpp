#ifndef SPHERE_H
#define SPHERE_H

#include "material.hpp"
#include "objects.hpp"
#include "renderer.hpp"
#include "common/common.hpp"

#include<cmath>

class Sphere : public Visible{
public:
  Sphere(const Point3& _center, const double& r):Sphere(_center,r,nullptr) {}
  Sphere(const Point3& _center, const double& r, Material* _material) : center(_center), radius(r), material(_material) {
    double r_abs = fabs(r);
    Vector3 r_vec(r_abs,r_abs,r_abs);
    bbox = AABB(_center + r_vec, _center - r_vec);
  }

  AABB Get_Bounding_box(void) const override{ return bbox; }

  virtual Color Ray_Color(const Ray& r, const Hit_record& rec) const override{
    if(material == nullptr){	// No material attatched, use default scatter
      Vector3 scatter_direction = Ray::Scatter_Direction(rec.front_face?rec.normal:-rec.normal);
      return 0.64 * Renderer::Instance()->Ray_Color(Ray(rec.position, scatter_direction), rec.hit_counts);
    } else return material->Ray_Color(r,rec); 
  }
  
  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const override{
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
  Material* material;

  AABB bbox;
};

#endif
