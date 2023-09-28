#include "plane.hpp"

#include<material.hpp>
#include<util/util.hpp>

#include<utility>
#include<cmath>

Plane::Plane() : bbox(AABB(Point3(1e10,1e10,0),Point3(-1e10,-1e10,0)).Pad()),
	  Visible(),
	  material(nullptr) {}

Plane& Plane::Set_Position(const Point3& Q, const Point3& A, const Point3& B){
  Vector3 u=A-Q, v=B-Q;
  refframe = Coordinate3().Set_Translation(Coordinate3::Origin(Q)).Set_Rotation(Coordinate3::AlignXY(u,v));
  bbox = refframe.Local2World(AABB(Point3(1e10,1e10,0),Point3(-1e10,-1e10,0)).Pad());
  return *this;
}


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
