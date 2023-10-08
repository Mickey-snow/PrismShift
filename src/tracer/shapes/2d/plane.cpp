#include "plane.hpp"

#include<util/util.hpp>

AABB Plane::Get_Bbox(void) const{
  static const AABB box = AABB(Interval<double>::Universe(), Interval<double>::Universe(), Interval<double>()).Pad();
  return box;
}

Hit_record Plane::Hit(const Ray& r, const Interval<double>& time_interval) const{
  double time = -r.Origin().z() / r.Direction().z();
  if(std::isnan(time)) return Hit_record{};
  if(!time_interval.Contains(time)) return Hit_record{};

  static const Normal normal{0,0,1};
  return Hit_record::ORTN(
			  this,
			  r,
			  time,
			  normal
			  );
}
