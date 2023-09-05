#include "plane.hpp"

#include<material.hpp>
#include<util/util.hpp>

#include<utility>
#include<cmath>

Color Plane::Ray_Color(const Ray& r, const Hit_record& rec) const{
  if(material == nullptr) return Color(1,0,0);
  else return material->Ray_Color(r,rec);
}

Hit_record Plane::Ray_Hit(const Ray& r, const Interval<double>& time_interval) const{
  Hit_record rec; rec.hits = false;

  auto [hits,time] = Intersection(r,time_interval);
  auto intersection = r.At(time);
  if(!hits) return rec;

  rec.hits = true;
  rec.time = time;
  rec.hitted_obj = Get_ptr();
  rec.position = intersection;
  rec.Set_Face_Normal(r,normal);

  return rec;
}

std::pair<bool, double> Plane::Intersection(const Ray& r,const Interval<double>& time_interval) const{
  static double EPS = 1e-6;
  double tup, tdown,time;
  tup = dist_to_orig - Vector3::Dot(r.Origin(), normal);
  tdown = Vector3::Dot(r.Direction(), normal);
  if(fabs(tdown) < EPS) return std::make_pair(false,-1); // Ray r parallel to plane
  time = tup / tdown;
  if(!time_interval.Contains(time)) return std::make_pair(false, -1);

  return std::make_pair(true, time);
}

std::pair<double,double> Plane::Decomposition(const Vector3& p)const{
  double alpha = Vector3::Dot(Omega, Vector3::Cross(u,p));
  double beta = Vector3::Dot(Omega, Vector3::Cross(p,v));
  return std::make_pair(alpha, beta);
}

Point2 Plane::Map_Texture(const Ray& r, const Hit_record& rec) const{
  auto [alpha,beta] = Decomposition(rec.position - Q);
  return Point2(beta-floor(beta), alpha-floor(alpha));
}

bool Plane::On_Object(const double& alpha, const double& beta) const{
  return true;
}
