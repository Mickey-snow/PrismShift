#ifndef UTILITY_GEOMETRY_H
#define UTILITY_GEOMETRY_H

#include<cassert>
#include<utility>
#include<format>
#include<cmath>
#include<iostream>
#include<functional>
#include<iterator>
#include<type_traits>
#include<concepts>

#include "util/vecmath.hpp"
#include "util/geometry_fwd.hpp"

class ITransformation;

template<std::size_t N>
class Vector : public basic_vector<double, N>{
private:
  using super = basic_vector<double, N>;
  static constexpr auto EPS = super::EPS;
public:
  using super::super;
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;

  template<vector_like T>
  explicit Vector<N>(T&& it) : super(std::forward<T>(it)) {}

  static Vector<N> Normalized(const Vector<N>& v){ return v / v.Length(); }
  Vector<N> Normalized(void) const{ return Vector<N>::Normalized(*this); }

  bool isUnit(void) const{ return fabs(super::Length_squared()-1.0) < EPS; }

  template<vector_like T, vector_like U>
  static double Dot(const T& lhs, const U& rhs)
    requires (T::dimension == N && U::dimension == N){
    double ans = 0;
    for(std::size_t i=0;i<dimension;++i) ans += lhs.v[i] * rhs.v[i];
    return ans;
  }
  template<vector_like T, vector_like U>
  static Vector<3> Cross(const T& lhs, const U& rhs)
    requires (T::dimension==3 && U::dimension==3 && N==3){
    return Vector<3>(lhs.v[1] * rhs.v[2] - lhs.v[2] * rhs.v[1],
		   lhs.v[2] * rhs.v[0] - lhs.v[0] * rhs.v[2],
		   lhs.v[0] * rhs.v[1] - lhs.v[1] * rhs.v[0]);
  }
  template<vector_like T, vector_like U>
  static Vector<3> Cross(const T& lhs, const U& rhs)
    requires(T::dimension==2 && U::dimension==2 && N==2){
    return Vector<3>(0,0,
		     lhs.v[0]*rhs.v[1] - lhs.v[1]*rhs.v[0]);
  }
  double Dot(const vector_like auto& rhs) const{ return Dot(*this, rhs); }
  Vector<N> Cross(const vector_like auto& rhs) const{ return Cross(*this, rhs); }

  static bool isPerpendicular(const Vector<N>& a,const Vector<N>& b){
    return fabs(Vector<N>::Dot(a,b)) < EPS;
  }

  static bool Same_Direction(const Vector<N>& a,const Vector<N>& b){
    if(a.Near_Zero() || b.Near_Zero()) return true;
    double t;
    for(std::size_t i=0;i<dimension;++i)
      if(b[i] != 0) { t = a[i]/b[i]; break; }
    return b*t == a;
  }
  
  static Vector<N> Random_Unit(void);
  static Vector<N> Random(const double& min, const double& max);

  Vector<N> Transform(const ITransformation&) const;
};


template<std::size_t N>
class Point : public basic_vector<double, N>{
private:
  using super = basic_vector<double,N>;
public:
  using super::super;
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;

  template<vector_like T>
  explicit Point<N>(T&& it) : super(std::forward<T>(it)) {}

  Point<N> Transform(const ITransformation&) const;

  Point<N> operator + (const Vector<N>& rhs) const{ return
      (Point<N>)vector_arithmetic(*this, rhs, std::plus<>{});
  }
  friend Point<N> operator + (const Vector<N>& lhs, const Point<N>& rhs){
    return (Point<N>)vector_arithmetic(lhs,rhs, std::plus<>{});
  }
  Vector<N> operator - (const Point<N>& rhs) const{
    return (Vector<N>)vector_arithmetic(*this, rhs, std::minus<>{});
  }
  Point<N> operator - (const Vector<N>& rhs) const{
    return (Point<N>)vector_arithmetic(*this, rhs, std::minus<>{});
  }
};


class Normal : public basic_vector<double,3>{
private:
  using super = basic_vector;
public:
  using value_type = super::value_type;
  static constexpr auto dimension = super::dimension;
  
  template<typename... Ts>
  Normal(Ts&&... param) : super(std::forward<Ts>(param)...) { Normalize(); }

  Normal(const Normal& it) : super(it) { Normalize(); }
  Normal(Normal&& it) : super(std::move(it)) { Normalize(); }
  ~Normal() = default;
  template<vector_like T>
  explicit Normal(T&& it) : super(std::forward<T>(it)) { Normalize(); }
  Normal& operator = (const Normal& it){
    Normal tmp(it); this->swap(tmp);
    return *this;
  }
  Normal& operator = (Normal&& it){
    Normal tmp(std::move(it)); this->swap(tmp);
    return *this;
  }

  Normal Transform(const ITransformation&) const;

  Normal operator +(const Vector3& rhs) const{
    return (Normal)vector_arithmetic(*this, rhs, std::plus<>{});
  }
  Normal operator -(const Vector3& rhs) const{
    return (Normal)vector_arithmetic(*this, rhs, std::minus<>{});
  }
private:
  void Normalize(){
    auto length = super::Length();
    for(auto& it : v) it /= length;
  }
};


#endif
