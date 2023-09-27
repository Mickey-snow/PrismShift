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

  const Normal normal{0,0,1};
  return Hit_record::MakeHitRecordWith_ORTPN(this,
					     r,
					     time,
					     r.At(time),
					     normal);
}


Point2 Plane::Map_Texture(const Hit_record& rec) const{
  auto alpha = rec.position.x();
  auto beta = rec.position.y();
  return Point2(beta-floor(beta), alpha-floor(alpha));
}
