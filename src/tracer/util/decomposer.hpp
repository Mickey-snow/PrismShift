#ifndef DECOMPOSER_H
#define DECOMPOSER_H

#include "geometry.hpp"
#include<array>

class Decomposer3d{
public:
  template<typename A, typename B, typename C>
  Decomposer3d(const internal::BaseVec3<A>& a,const internal::BaseVec3<B>& b, const internal::BaseVec3<C>& c) : Decomposer3d(Vector3(a), Vector3(b), Vector3(c)) {}
  Decomposer3d(const Vector3& a,const Vector3& b, const Vector3& c) : i(a),j(b),k(c) { Init(); }
  Decomposer3d(const Vector3& a,const Vector3& b) : Decomposer3d(a,b,Vector3::Cross(a,b)) {}

  template<typename T>
  double Componenti(const internal::BaseVec3<T>& p) const {
    return Vector3::Dot(nalpha, Vector3::Cross(k,(Vector3)p));
  }
  template<typename T>
  double Componentj(const internal::BaseVec3<T>& p) const {
    return Vector3::Dot(nbeta, Vector3::Cross(i,(Vector3)p));
  }
  template<typename T>
  double Componentk(const internal::BaseVec3<T>& p) const {
    return Vector3::Dot(ngamma, Vector3::Cross(j,(Vector3)p));
  }

  std::array<double,3> Get_Component(const Vector3& p) const {
    return std::array<double,3>{
      Componenti(p),
      Componentj(p),
      Componentk(p)};
  }
private:
  Vector3 i,j,k;
  Vector3 nbeta,nalpha,ngamma;

  void Init(){
    nalpha = j/Vector3::Dot(j, Vector3::Cross(k,i));
    nbeta = k/Vector3::Dot(k, Vector3::Cross(i,j));
    ngamma = i/Vector3::Dot(i, Vector3::Cross(j,k));
  }
};

#endif
