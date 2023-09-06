#include "triangle.hpp"

#include<util/util.hpp>

#include<cmath>

Color Triangle::Ray_Color(const Hit_record &rec) const{
  if(material == nullptr) return Color(1,0,0);
  else return material->Ray_Color(rec);
}

Hit_record Triangle::Ray_Hit(const Ray& r, const Interval<double>& time_interval) const{
  double time = r.intersectionTimeWithPlane(Q,u,v);
  if(std::isnan(time)) return Hit_record::NoHit();
  if(!time_interval.Surrounds(time)) return Hit_record::NoHit();
  auto intersection = r.At(time);

  Vector3 p = intersection - Q;
  auto alpha = planeDec->Componenti(p);
  auto beta = planeDec->Componentj(p);
  if(!On_Object(alpha,beta)) return Hit_record::NoHit();

  auto ww = normalDec->Componenti(intersection);
  auto wu = normalDec->Componentj(intersection);
  auto wv = normalDec->Componentk(intersection);
  auto weightedNormal = (nw*ww+nu*wu+nv*wv) / (ww+wu+wv);
    
  return Hit_record::MakeHitRecordWith_ORTPN(std::make_shared<Triangle>(*this),
					     r,
					     time,
					     intersection,
					     weightedNormal);
}

Point2 Triangle::Map_Texture(const Hit_record& rec) const{
  Vector3 p = rec.position - Q;
  auto alpha = planeDec->Componenti(p);
  auto beta = planeDec->Componentj(p);
  return Point2(alpha,beta);
}
