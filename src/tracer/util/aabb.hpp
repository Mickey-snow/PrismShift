#ifndef AABB_H
#define AABB_H

#include "geometry.hpp"
#include "ray.hpp"
#include "interval.hpp"


#include<cmath>

class AABB{
  // Approximate an visible object to a cube
  // All rays miss this cube thus miss the object
public:
  AABB():x_interval(Interval<double>::Empty()), y_interval(Interval<double>::Empty()), z_interval(Interval<double>::Empty()) {}
  AABB(const Interval<double>& x,const Interval<double>& y, const Interval<double>& z) : x_interval(x), y_interval(y), z_interval(z) {}
  AABB(const Point3& a, const Point3& b){
    x_interval = Interval(fmin(a.x(),b.x()), fmax(a.x(),b.x()));
    y_interval = Interval(fmin(a.y(),b.y()), fmax(a.y(),b.y()));
    z_interval = Interval(fmin(a.z(),b.z()), fmax(a.z(),b.z()));
  }
  AABB(const AABB& a,const AABB& b):
    x_interval(Interval(a.x_interval,b.x_interval)),
    y_interval(Interval(a.y_interval,b.y_interval)),
    z_interval(Interval(a.z_interval,b.z_interval)) {}

  bool operator == (const AABB& rhs) const{
    return x_interval == rhs.Axis(0) &&
      y_interval == rhs.Axis(1) &&
      z_interval == rhs.Axis(2);
  }
  
  const Interval<double>& Axis(const int& n) const;

  bool Is_Hit_in_Interval(const Ray&, Interval<double>) const;

  AABB Pad() const;

  class Componentbased_Comparer{
  public:
    Componentbased_Comparer(const int& _component) : component(_component) {}
    bool operator () (const AABB& a, const AABB& b) const{
      return a.Axis(component).begin < b.Axis(component).begin;
    }
  private:
    int component;
  };
private:
  Interval<double> x_interval,y_interval,z_interval;
};

#endif
