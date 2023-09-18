#include "parallelogram.hpp"

#include<util/util.hpp>
#include<utility>

Hit_record Parallelogram::Ray_Hit(const Ray& rw, const Interval<double>& time_interval) const{
  const Ray r = refframe.World2Local(rw);

  double time = -r.Origin().z() / r.Direction().z();
  if(std::isnan(time)) return Hit_record::NoHit();
  if(!time_interval.Surrounds(time)) return Hit_record::NoHit();

  auto hit_point = r.At(time);
  if(!On_Object(hit_point.x(), hit_point.y())) return Hit_record::NoHit();

  return Hit_record::MakeHitRecordWith_ORTPN(this,
					     r,
					     time,
					     hit_point,
					     normal);
}

Point2 Parallelogram::Map_Texture(const Hit_record& rec) const{
  auto alpha = rec.position.y();
  auto beta = rec.position.x();

  return Point2(alpha,beta);
}
