#include "interval.hpp"



template<>
Interval<double> Interval<double>::Positive(){ return Interval<double>(1e-5, 1e5); }
template<>
Interval<double> Interval<double>::Universe(){ return Interval<double>(-1e5, 1e5); }
