#include "aabb.hpp"

#include<stdexcept>
#include<format>
#include<memory>

const Interval<double>& AABB::Axis(const int& n) const{
    if(n == 0) return x_interval;
    if(n == 1) return y_interval;
    if(n == 2) return z_interval;
    throw std::range_error(std::format("Range Error: AABB::Axis(n) accepts n=0,1,2 for x,y,z interval components. given n={}", n));
}

bool AABB::Is_Hit_in_Interval(const Ray& r, Interval<double> ray_t)const {
  for(int a=0;a<3;++a){
    auto invD = 1 / r.Direction()[a];
    auto orig = r.Origin()[a];

    auto t0 = (Axis(a).begin - orig) * invD;
    auto t1 = (Axis(a).end - orig) * invD;

    if(invD < 0) std::swap(t0,t1);

    // Overlap the (a) component's interval [t0,t1]
    // Let ray_t be the new effective time interval
    if(t0 > ray_t.begin) ray_t.begin = t0;
    if(t1 < ray_t.end) ray_t.end = t1;

    if(ray_t.end <= ray_t.begin) return false;
  }
  return true;
}

AABB AABB::Pad() const{
  static double EPS = 1e-3;
  Interval<double> xx = (x_interval.Size() < EPS) ? x_interval.Expand(EPS) : x_interval;
  Interval<double> yy = (y_interval.Size() < EPS) ? y_interval.Expand(EPS) : y_interval;
  Interval<double> zz = (z_interval.Size() < EPS) ? z_interval.Expand(EPS) : z_interval;
  return AABB(xx,yy,zz);
}
