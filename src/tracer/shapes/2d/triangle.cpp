#include "triangle.hpp"

#include<util/util.hpp>

#include<cmath>

Triangle::Triangle() : bbox(AABB(Point3(0,0,0), Point3(1,1,0)).Pad()),
	     Visible(){
  nu = nv = nw = Normal{0,0,1};
}
	     
Triangle& Triangle::Set_Position(const Point3& Q, const Point3& A, const Point3& B){
  Vector3 u=A-Q, v=B-Q;
  refframe = Coordinate3().Set_Translation(Coordinate3::Origin(Q)).Set_Rotation(Coordinate3::AlignXY(u,v));
  bbox = refframe.Local2World(AABB(Point3(0,0,0), Point3(1,1,0)).Pad());
  return *this;
}

Triangle& Triangle::Set_Normals(const Normal& u, const Normal& v, const Normal& w){
  nu=u; nv=v; nw=w;
  return *this;
}


Hit_record Triangle::Ray_Hit(const Ray& rw, const Interval<double>& time_interval) const{
  auto On_Object = [](const double& a,const double& b){
    return (a+b)<=1 && 0<=a && 0<=b;
  };
  
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
