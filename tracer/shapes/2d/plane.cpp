#include "plane.hpp"

#include<material.hpp>
#include<util/util.hpp>

#include<utility>
#include<cmath>

Color Plane::Ray_Color(const Hit_record& rec) const{
  if(material == nullptr) return Color(1,0,0);
  else return material->Ray_Color(rec);
}

Hit_record Plane::Ray_Hit(const Ray& r, const Interval<double>& time_interval) const{
  double time = r.intersectionTimeWithPlane(Q,u,v);
  if(std::isnan(time)) return Hit_record::NoHit();
  if(!time_interval.Contains(time)) return Hit_record::NoHit();

  return Hit_record::MakeHitRecordWith_ORTPN(std::make_shared<Plane>(*this),
					     r,
					     time,
					     r.At(time),
					     normal);
}
Point2 Plane::Map_Texture(const Hit_record& rec) const{
  Vector3 p = rec.position-Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  return Point2(beta-floor(beta), alpha-floor(alpha));
}
