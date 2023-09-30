#include "aabb.hpp"

#include<stdexcept>
#include<format>
#include<memory>
#include<functional>
#include<vector>
#include<algorithm>

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


AABB AABB::Transform(const Transformation& tr) const{
  std::vector<Point3> v;
  std::function<double(const Interval<double>&)> vertex_component[] = {
    [](const Interval<double>& i){ return i.begin; },
    [](const Interval<double>& i){ return i.end; }
  };
  for(int i=0;i<2;++i) for(int j=0;j<2;++j) for(int k=0;k<2;++k) v.emplace_back(Point3{vertex_component[i](Axis(0)), vertex_component[j](Axis(1)), vertex_component[k](Axis(2)) });

  for(auto& it : v) it = it.Transform(tr);

  return AABB(AABB(AABB(v[0],v[1]),AABB(v[2],v[3])),AABB(AABB(v[4],v[5]),AABB(v[6],v[7])));
}
