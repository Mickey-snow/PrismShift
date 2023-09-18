#include "parallelogram.hpp"

#include<util/util.hpp>
#include<utility>

Hit_record Parallelogram::Ray_Hit(const Ray& rw, const Interval<double>& time_interval) const{
  const Ray r = refframe.World2Local(rw);

  double time = -r.Origin().z() / r.Direction().z();
  if(std::isnan(time)) return Hit_record::NoHit();
  if(!time_interval.Surrounds(time)) return Hit_record::NoHit();

  auto intersection = r.At(time);
  if(!On_Object(intersection.x(), intersection.y())) return Hit_record::NoHit();

  return Hit_record::MakeHitRecordWith_ORTPN(this,
					     rw,
					     time,
					     refframe.Local2World(intersection),
					     normal);
}

Point2 Parallelogram::Map_Texture(const Hit_record& rec) const{
  Vector3 p = rec.position - Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  return Point2(alpha,beta);
}
