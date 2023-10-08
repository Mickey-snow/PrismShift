#include "parallelogram.hpp"

#include<util/util.hpp>

#include<cmath>


AABB Parallelogram::Get_Bbox() const{
  static const AABB box = AABB(Point3(0,0,0), Point3(1,1,1)).Pad();
  return box;
}


Hit_record Parallelogram::Hit(const Ray& r, const Interval<double>& time_interval) const{
  static auto On_Object = [](const double& a,const double& b){
    return 0<=a&&a<=1 && 0<=b&&b<=1;
  };
  

  double time = -r.Origin().z() / r.Direction().z();
  if(std::isnan(time)) return Hit_record{};
  if(!time_interval.Surrounds(time)) return Hit_record{};

  auto hit_point = r.At(time);
  if(!On_Object(hit_point.x(), hit_point.y())) return Hit_record{};

  static const Normal normal{0,0,1};
  return Hit_record::ORTN(
			  this,
			  r,
			  time,
			  normal
			  );
}
