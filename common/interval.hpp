#ifndef INTERVAL_H
#define INTERVAL_H

#include<limits>

template<typename T>
class Interval{
private:
  const T Positive_Inf = std::numeric_limits<T>::max();
  const T Negative_Inf = std::numeric_limits<T>::min();
  
public:
  T begin,end;
  
  Interval():begin(Positive_Inf),end(Negative_Inf) {} // default interval is empty
  Interval(const T& _min, const T& _max) : begin(_min), end(_max) {}
  ~Interval() = default;

  bool isEmpty(void) const{ return begin>end; }
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
  static Interval<T> Universe(){ return Interval<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max()); }
};


#endif
