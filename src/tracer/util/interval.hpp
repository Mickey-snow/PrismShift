#pragma once

#include <iostream>

template <typename T>
class Interval {
 private:
  static constexpr double EPS = 1e-8;

 public:
  T begin, end;

  explicit constexpr Interval() : begin(0), end(0) {}
  constexpr Interval(const T& _min, const T& _max) : begin(_min), end(_max) {}
  constexpr Interval(const Interval& a, const Interval& b) {
    begin = a.begin < b.begin ? a.begin : b.begin;
    end = a.end > b.end ? a.end : b.end;
  }

  constexpr bool operator==(const Interval& rhs) const {
    return (begin - EPS < rhs.begin && rhs.begin < begin + EPS) &&
           (end - EPS < rhs.end && rhs.end < end + EPS);
  }

  constexpr Interval Expand(double x) const {
    return Interval(begin - x, end + x);
  }

  constexpr T Size() const { return end - begin; }
  constexpr bool isEmpty() const { return Size() <= 0; }
  constexpr bool In(const Interval& a) const {
    return a.begin <= begin and end <= a.end;
  }

  constexpr bool Contains(const T& x) const {
    return begin - EPS < x and x < end + EPS;
  }
  constexpr bool Surrounds(const T& x) const { return begin < x and x < end; }

  constexpr T Clamp(const T& x) const {
    if (this->Contains(x))
      return x;
    return x < begin ? begin : end;
  }

  friend std::ostream& operator<<(std::ostream& out, const Interval<T>& seq) {
    return out << '[' << seq.begin << ',' << seq.end << ']';
  }

  static constexpr Interval<T> Empty = Interval<T>(T{}, T{});
  static Interval<T> Positive();
  static Interval<T> Universe();
};
