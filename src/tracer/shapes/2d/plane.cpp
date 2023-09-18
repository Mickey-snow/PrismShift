#include "plane.hpp"

#include<material.hpp>
#include<util/util.hpp>

#include<utility>
#include<cmath>

Hit_record Plane::Ray_Hit(const Ray& rw, const Interval<double>& time_interval) const{
  const Ray r = refframe.World2Local(rw);

  double time = -r.Origin().z() / r.Direction().z();
  if(std::isnan(time)) return Hit_record::NoHit();
  if(!time_interval.Contains(time)) return Hit_record::NoHit();

  return Hit_record::MakeHitRecordWith_ORTPN(this,
					     rw,
					     time,
					     rw.At(time),
					     normal);
}


Point2 Plane::Map_Texture(const Hit_record& rec) const{
  Vector3 p = rec.position-Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  return Point2(beta-floor(beta), alpha-floor(alpha));
}
