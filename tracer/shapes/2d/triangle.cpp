#include "triangle.hpp"

#include<util/util.hpp>

#include<cmath>

Color Triangle::Ray_Color(const Ray &r, const Hit_record &rec) const{
  if(material == nullptr) return Color(1,0,0);
  else return material->Ray_Color(r,rec);
}

Hit_record Triangle::Ray_Hit(const Ray& r, const Interval<double>& time_interval) const{
  Hit_record rec_miss,rec; rec_miss.hits = false;

  double time = r.intersectionTimeWithPlane(Q,u,v);
  if(std::isnan(time)) return rec_miss;
  if(!time_interval.Surrounds(time)) return rec_miss;
  auto intersection = r.At(time);

  Vector3 p = intersection - Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  if(!On_Object(alpha,beta)) return rec_miss;

  rec.hits = true;
  rec.time = time;
  rec.hitted_obj = std::make_shared<Triangle>(*this);
  rec.position = intersection;
  rec.Set_Face_Normal(r, normal);

  return rec;
}

Point2 Triangle::Map_Texture(const Ray& r, const Hit_record& rec) const{
  Vector3 p = rec.position - Q;
  auto alpha = decomposer->Componenti(p);
  auto beta = decomposer->Componentj(p);
  return Point2(alpha,beta);
}
