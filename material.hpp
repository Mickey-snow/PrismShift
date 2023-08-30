#ifndef MATERIAL_H
#define MATERIAL_H

#include "objects.hpp"
#include "renderer.hpp"
#include "common/common.hpp"

class Material{
public:
  ~Material() = default;

  virtual Color Ray_Color(const Ray&, const Hit_record&) const = 0;
};

class Lambertian : public Material{
public:
  Lambertian(const Color& col) : attenuation(col) {}

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override{
    Vector3 scattered_direction = Ray::Scatter_Direction(rec.front_face ? rec.normal : -rec.normal);
    return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, scattered_direction), rec.hit_counts);
  }
  
protected:
  Color attenuation;
};

class Metal : public Material{
public:
  Metal(const Color& col, const double& f=0) : attenuation(col),fuzz(fabs(f)<1?f:1) {}

  virtual Color Ray_Color(const Ray& r,const Hit_record& rec) const override{
    Vector3 reflected_direction = Ray::Reflect_Direction(r.Direction(), rec.front_face ? rec.normal : -rec.normal);
    reflected_direction = reflected_direction + fuzz*Vector3::Random_Unit();
    return attenuation * Renderer::Instance()->Ray_Color(Ray(rec.position, reflected_direction), rec.hit_counts);
  }
protected:
  Color attenuation;
  double fuzz;
};

#endif
