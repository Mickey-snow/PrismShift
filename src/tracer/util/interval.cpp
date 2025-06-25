#include "interval.hpp"

template <>
Interval<Float> Interval<Float>::Positive() {
  return Interval<Float>(1e-8, 1e8);
}
template <>
Interval<Float> Interval<Float>::Universe() {
  return Interval<Float>(-1e8, 1e8);
}
