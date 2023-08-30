#ifndef INTERVAL_H
#define INTERVAL_H

#include "constant.hpp"

class Interval{
public:
  Interval():begin(+infinity),end(-infinity) {} // default interval is empty
  Interval(const double& _min, const double& _max) : begin(_min), end(_max) {}
  ~Interval() = default;
  
  bool Contains(const double& x) const{
    return begin<=x and x<=end;
  }
  bool Surrounds(const double& x) const{
    return begin<x and x<end;
  }

  double Clamp(const double& x) const{
    if(this->Contains(x)) return x;
    return x<begin ? begin : end;
  }
  
  static const Interval Empty_Interval, Universe_Interval;

  double begin,end;
};

const static Interval Empty_Interval(+infinity, -infinity);
const static Interval Universe_Interval(-infinity, +infinity);

#endif
