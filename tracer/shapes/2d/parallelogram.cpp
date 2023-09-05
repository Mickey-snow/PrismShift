#include "parallelogram.hpp"

#include<util/util.hpp>
#include<utility>

Color Parallelogram::Ray_Color(const Ray &r, const Hit_record &rec) const{
  if(material == nullptr) return Color(0,1,0);
  else return material->Ray_Color(r,rec);
}

Hit_record Parallelogram::Ray_Hit(const Ray& r, const Interval<double>& time_interval) const{
  double time = r.intersectionTimeWithPlane(Q,u,v);

  if(std::isnan(time)) return Hit_record::NoHit();
  if(!time_interval.Surrounds(time)) return Hit_record::NoHit();
  auto intersection = r.At(time);

  Vector3 p = intersection - Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  if(!On_Object(alpha,beta)) return Hit_record::NoHit();

  return Hit_record::MakeHitRecordWith_ORTPN(std::make_shared<Parallelogram>(*this),
					     r,
					     time,
					     intersection,
					     normal);
}

Point2 Parallelogram::Map_Texture(const Ray& r, const Hit_record& rec) const{
  Vector3 p = rec.position - Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  return Point2(alpha,beta);
}
