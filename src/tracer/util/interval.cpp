#include "interval.hpp"

template <>
Interval<double> Interval<double>::Positive() {
  return Interval<double>(1e-8, 1e8);
}
template <>
Interval<double> Interval<double>::Universe() {
  return Interval<double>(-1e8, 1e8);
}
