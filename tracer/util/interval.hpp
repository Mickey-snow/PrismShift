#ifndef INTERVAL_H
#define INTERVAL_H

#include<iostream>
#include<limits>

template<typename T>
class Interval{
public:
  T begin,end;
  
  Interval():begin(std::numeric_limits<T>::max()),end(std::numeric_limits<T>::min()) {} // default interval is empty
  Interval(const T& _min, const T& _max) : begin(_min), end(_max) {}
  Interval(const Interval& a, const Interval& b){
    begin = a.begin < b.begin ? a.begin : b.begin;
    end = a.end > b.end ? a.end : b.end;
  }
  ~Interval() = default;

  bool operator == (const Interval& rhs) const{
    static double EPS = 1e-8;
    return (begin-EPS<rhs.begin && rhs.begin<begin+EPS) &&
      (end-EPS<rhs.end && rhs.end<end+EPS);
  }

  Interval(const Interval& cp):begin(cp.begin), end(cp.end) {}
  Interval& operator = (const Interval& cp){
    begin = cp.begin; end = cp.end;
    return *this;
  }

  Interval Expand(const double& x) const{
    return Interval(begin-x, end+x);
  }

  T Size(void) const{ return end-begin; }
  bool isEmpty(void) const{ return Size()<=0; }
  bool In(const Interval& a) const{ return a.begin<=begin and end<=a.end; }
  
  bool Contains(const T& x) const{
    return begin<=x and x<=end;
  }
  bool Surrounds(const T& x) const{
    return begin<x and x<end;
  }

  T Clamp(const T& x) const{
    if(this->Contains(x)) return x;
    return x<begin ? begin : end;
  }

  friend std::ostream& operator << (std::ostream& out, const Interval<T>& seq){
    return out<<'['<<seq.begin<<','<<seq.end<<']';
  }

  static Interval<T> Empty(){ return Interval<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::min()); }
  static Interval<T> Positive(){ return Interval<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max()); }
};


#endif
