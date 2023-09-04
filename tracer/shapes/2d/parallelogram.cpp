#include "parallelogram.hpp"

#include<util/util.hpp>
#include<material.hpp>

#include<cmath>

/*
Color Parallelogram::Ray_Color(const Ray &r, const Hit_record &rec) const{
  if(material == nullptr) return Color(0,0,0);
  else return material->Ray_Color(r,rec);
}

Hit_record Parallelogram::Ray_Hit(const Ray& r, const Interval<double>& time) const{
  Hit_record rec; rec.hits = false;
  
  static double EPS = 1e-7;
  double tup, tdown;
  tup = dist_to_orig - Vector3::Dot(r.Origin(), normal);
  tdown = Vector3::Dot(r.Direction(), normal);
  if(fabs(tdown) < EPS) return rec; // Ray r parallel to plane
  rec.time = tup / tdown;
  if(!time.Contains(rec.time)) return rec;

  Point3 intersection = r.At(rec.time);
  Vector3 p = intersection-Q;
  double alpha = Vector3::Dot(Omega, Vector3::Cross(u,p));
  double beta = Vector3::Dot(Omega, Vector3::Cross(p,v));

  if(!(0<=alpha && alpha<=1 && 0<=beta && beta<=1)) return rec;

  rec.hits = true;
  rec.hitted_obj = std::make_shared<Parallelogram>(*this);
  rec.position = intersection;
  rec.Set_Face_Normal(r, normal);

  return rec;
}

Point2 Parallelogram::Map_Texture(const Ray& r, const Hit_record& rec) const{
  Vector3 p = rec.position-Q;
  double alpha = Vector3::Dot(Omega, Vector3::Cross(u,p));
  double beta = Vector3::Dot(Omega, Vector3::Cross(p,v));

  return Point2(beta, alpha);
}
*/
