#include "parallelogram.hpp"

#include<util/util.hpp>
#include<utility>

Parallelogram::Parallelogram() : bbox(AABB(Point3(0,0,0), Point3(1,1,0)).Pad()),
				 Visible(),
				 material(nullptr) {}

Parallelogram& Parallelogram::Set_Position(const Point3& Q, const Point3& A, const Point3& B){
  Vector3 u=A-Q, v=B-Q;
  refframe = Coordinate3().Set_Translation(Coordinate3::Origin(Q)).Set_Rotation(Coordinate3::AlignXY(u,v));
  bbox = refframe.Local2World(AABB(Point3(0,0,0), Point3(1,1,0)).Pad());
  return *this;
}



Hit_record Parallelogram::Ray_Hit(const Ray& rw, const Interval<double>& time_interval) const{
  auto On_Object = [](const double& a,const double& b){
    return 0<=a&&a<=1 && 0<=b&&b<=1;
  };

  
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
					     Normal{0,0,1});
}

Point2 Parallelogram::Map_Texture(const Hit_record& rec) const{
  auto alpha = rec.position.y();
  auto beta = rec.position.x();

  return Point2(alpha,beta);
}
