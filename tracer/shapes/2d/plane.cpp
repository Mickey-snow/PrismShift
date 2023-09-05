#include "plane.hpp"

#include<material.hpp>
#include<util/util.hpp>

#include<utility>
#include<cmath>

Color Plane::Ray_Color(const Ray& r, const Hit_record& rec) const{
  if(material == nullptr) return Color(1,0,0);
  else return material->Ray_Color(r,rec);
}

Hit_record Plane::Ray_Hit(const Ray& r, const Interval<double>& time_interval) const{
  Hit_record rec; rec.hits = false;

  double time = r.intersectionTimeWithPlane(Q,u,v);
  if(std::isnan(time)) return rec;
  if(!time_interval.Contains(time)) return rec;

  auto intersection = r.At(time);

  rec.hits = true;
  rec.time = time;
  rec.hitted_obj = std::make_shared<Plane>(*this);
  rec.position = intersection;
  rec.Set_Face_Normal(r,normal);

  return rec;
}
Point2 Plane::Map_Texture(const Ray& r, const Hit_record& rec) const{
  Vector3 p = rec.position-Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  return Point2(beta-floor(beta), alpha-floor(alpha));
}
