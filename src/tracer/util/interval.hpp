#pragma once

#include <iostream>

template <typename T>
class Interval {
 private:
  static constexpr double EPS = 1e-8;

 public:
  T begin, end;

  explicit Interval() : begin(0), end(0) {}
  Interval(const T& _min, const T& _max) : begin(_min), end(_max) {}
  Interval(const Interval& a, const Interval& b) {
    begin = a.begin < b.begin ? a.begin : b.begin;
    end = a.end > b.end ? a.end : b.end;
  }
  ~Interval() = default;

  bool operator==(const Interval& rhs) const {
    return (begin - EPS < rhs.begin && rhs.begin < begin + EPS) &&
           (end - EPS < rhs.end && rhs.end < end + EPS);
  }

  Interval(const Interval& cp) : begin(cp.begin), end(cp.end) {}
  Interval& operator=(const Interval& cp) {
    begin = cp.begin;
    end = cp.end;
    return *this;
  }

  Interval Expand(const double& x) const {
    return Interval(begin - x, end + x);
  }

  T Size(void) const { return end - begin; }
  bool isEmpty(void) const { return Size() <= 0; }
  bool In(const Interval& a) const { return a.begin <= begin and end <= a.end; }

  bool Contains(const T& x) const { return begin - EPS < x and x < end + EPS; }
  bool Surrounds(const T& x) const { return begin < x and x < end; }

  T Clamp(const T& x) const {
    if (this->Contains(x))
      return x;
    return x < begin ? begin : end;
  }

  friend std::ostream& operator<<(std::ostream& out, const Interval<T>& seq) {
    return out << '[' << seq.begin << ',' << seq.end << ']';
  }

  static Interval<T> Empty() { return Interval<T>(0, 0); }
  static Interval<T> Positive();
  static Interval<T> Universe();
};
