#include "triangle.hpp"

#include<util/util.hpp>

#include<cmath>


Hit_record Triangle::Ray_Hit(const Ray& rw, const Interval<double>& time_interval) const{
  const Ray r = refframe.World2Local(rw);

  double time = -r.Origin().z() / r.Direction().z();
  if(std::isnan(time)) return Hit_record::NoHit();
  if(!time_interval.Surrounds(time)) return Hit_record::NoHit();

  auto hit_point = r.At(time);
  if(!On_Object(hit_point.x(), hit_point.y())) return Hit_record::NoHit();
  
  auto wu = hit_point.x();
  auto wv = hit_point.y();
  auto ww = 1 - wu - wv;
  auto weightedNormal = (nw*ww+nu*wu+nv*wv);
    
  return Hit_record::MakeHitRecordWith_ORTPN(this,
					     r,
					     time,
					     hit_point,
					     weightedNormal);
}

Point2 Triangle::Map_Texture(const Hit_record& rec) const{
  auto alpha = rec.position.x();
  auto beta = rec.position.y();
  return Point2(alpha,beta);
}
