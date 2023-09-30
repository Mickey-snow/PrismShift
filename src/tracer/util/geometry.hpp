#ifndef UTILITY_GEOMETRY_H
#define UTILITY_GEOMETRY_H

#include<cassert>
#include<format>
#include<cmath>
#include<iostream>

class Transformation;

namespace internal{
  template<typename Child>
  class BaseVec2{
  protected:
    static constexpr double EPS = 1e-3;    
  public:
    // constructors
    BaseVec2(){ v[0]=v[1]=0; }
    BaseVec2(const double& e1,const double& e2){ v[0]=e1; v[1]=e2; }
    BaseVec2(std::initializer_list<double> li) : BaseVec2(){
      auto it = li.begin();
      for(int i=0;i<2&&it!=li.end();++i,++it)
	v[i] = *it;
    }
    
    // data access
    double& x() { return v[0]; }
    double& y() { return v[1]; }
    double x()  const{ return v[0]; }
    double y()  const{ return v[1]; }
    double operator[] (const int& i) const{ return v[i]; }
    double& operator[] (const int& i){ return v[i]; }

    // arithmetics
    Child operator -()  const { return Child{-v[0],-v[1]}; }
    Child operator +(const Child& rhs) const { return Child{v[0]+rhs[0], v[1]+rhs[1]}; }
    Child& operator += (const Child& rhs){ return *this = *this + rhs; }
    Child operator -(const Child& rhs) const { return *this + (-rhs); }
    Child& operator -= (const Child& rhs){ return *this = *this + (-rhs); }
    Child operator * (const double& rhs) const { return Child{v[0]*rhs, v[1]*rhs}; }
    friend Child operator * (const double& lhs,const Child& rhs){ return rhs*lhs; }
    Child& operator *= (const double& rhs){ return *this = *this * rhs; }
    Child operator / (const double& rhs) const{ return Child{v[0]/rhs, v[1]/rhs}; }
    Child& operator /= (const double& rhs){ return *this = *this / rhs; }
    bool operator == (const Child& rhs) const{ return fabs(v[0]-rhs.v[0])<EPS && fabs(v[1]-rhs.v[1])<EPS; }
    bool operator != (const Child& rhs) const{ return !(*this == rhs); }
    bool Near_Zero() const{ return fabs(v[0])<EPS && fabs(v[1])<EPS; }
    
    // debug
    bool hasnans(void) const{ return std::isnan(v[0]) || std::isnan(v[1]); }
    bool isvalid(void) const{ return !hasnans(); }

    double Length() const{ return std::sqrt(Length_squared()); }
    double Length_squared() const{ return v[0]*v[0]+v[1]*v[1]; }

    [[deprecated("use normalized instead")]]
    Child Normalize() const{ return *this / Length(); }
    Child Normalized() const{ return *this / Length(); }

    static double Dot(const Child& lhs, const Child& rhs){ return lhs.v[0]*rhs.v[0] + lhs.v[1]*rhs.v[1]; }
    double Dot(const Child& rhs) const{ return BaseVec2<Child>::Dot(*this, rhs); }
    
    double v[2];
  };


  
  template<typename Child>
  class BaseVec3{
  protected:
    static constexpr double EPS = 1e-3;
  public:
    // constructors
    BaseVec3(){ v[0]=v[1]=v[2]=0; }
    BaseVec3(const double& e1,const double& e2,const double& e3){ v[0]=e1; v[1]=e2; v[2]=e3; }
    BaseVec3(std::initializer_list<double> li) : BaseVec3(){
      auto it = li.begin();
      for(int i=0;i<3&&it!=li.end();++i,++it)
	v[i] = *it;
    }

    // data access
    double& x() { return v[0]; }
    double& y() { return v[1]; }
    double& z() { return v[2]; }
    double x()  const{ return v[0]; }
    double y()  const{ return v[1]; }
    double z()  const{ return v[2]; }
    double operator[] (const int& i) const{ return v[i]; }
    double& operator[] (const int& i){ return v[i]; }

    // arithmetics
    Child operator -()  const { return Child{-v[0],-v[1],-v[2]}; }
    Child operator +(const Child& rhs) const { return Child{v[0]+rhs[0], v[1]+rhs[1], v[2]+rhs[2]}; }
    Child& operator += (const Child& rhs){ return *this = *this + rhs; }
    Child operator -(const Child& rhs) const { return *this + (-rhs); }
    Child& operator -= (const Child& rhs){ return *this = *this + (-rhs); }
    Child operator * (const double& rhs) const { return Child{v[0]*rhs, v[1]*rhs, v[2]*rhs}; }
    friend Child operator * (const double& lhs,const Child& rhs){ return rhs*lhs; }
    Child& operator *= (const double& rhs){ return *this = *this * rhs; }
    Child operator / (const double& rhs) const{ return Child{v[0]/rhs, v[1]/rhs, v[2]/rhs}; }
    Child& operator /= (const double& rhs){ return *this = *this / rhs; }
    bool operator == (const Child& rhs) const{ return fabs(v[0]-rhs.v[0])<EPS && fabs(v[1]-rhs.v[1])<EPS && fabs(v[2]-rhs.v[2])<EPS; }
    bool operator != (const Child& rhs) const{ return !(*this == rhs); }
    bool Near_Zero() const{ return fabs(v[0])<EPS && fabs(v[1])<EPS && fabs(v[2])<EPS; }
    
    // debug
    bool hasnans(void) const{ return std::isnan(v[0]) || std::isnan(v[1]) || std::isnan(v[2]); }
    bool isvalid(void) const{ return !hasnans(); }

    bool isUnit(void) const{ return fabs(Length_squared()-1) < EPS; }

    double Length() const{ return std::sqrt(Length_squared()); }
    double Length_squared() const{ return v[0]*v[0]+v[1]*v[1]+v[2]*v[2]; }

    [[deprecated("use normalized instead")]]
    Child Normalize() const{ return *this / Length(); }
    Child Normalized() const{ return *this / Length(); }

    static double Dot(const Child& lhs, const Child& rhs){ return lhs.v[0]*rhs.v[0] + lhs.v[1]*rhs.v[1] + lhs.v[2]*rhs.v[2]; }
    double Dot(const Child& rhs) const{ return BaseVec3<Child>::Dot(*this, rhs); }
    static Child Cross(const Child& lhs, const Child& rhs){
             return Child(lhs.v[1] * rhs.v[2] - lhs.v[2] * rhs.v[1],
		   lhs.v[2] * rhs.v[0] - lhs.v[0] * rhs.v[2],
		   lhs.v[0] * rhs.v[1] - lhs.v[1] * rhs.v[0]);
    }
    Child Cross(const Child& rhs) const{ return BaseVec3<Child>::Cross(*this, rhs); }
    
    double v[3];
  };


  template<typename T>
  inline std::ostream& operator << (std::ostream& os, const BaseVec3<T>& v){
    os << std::format("({},{},{})", v[0],v[1],v[2]); return os;
  }
  template<typename T>
  inline std::ostream& operator << (std::ostream& os, const BaseVec2<T>& v){
  os << std::format("({},{})", v[0],v[1]); return os;
  }

} // namespace internal


class Vector3;
class Vector2;
class Point2;
class Point3;
class Normal;


class Vector3 : public internal::BaseVec3<Vector3>{
private:
  using Parent = internal::BaseVec3<Vector3>;
public:
  using Parent::Parent;

  template<typename T>
  explicit Vector3(const BaseVec3<T>& rhs){ v[0]=rhs.v[0]; v[1]=rhs.v[1]; v[2]=rhs.v[2]; }

  static Vector3 Normalized(const Vector3& v);
  Vector3 Normalized(void) const{ return Vector3::Normalized(*this); }

  double Dot(const Vector3& rhs) const{ return Parent::Dot(*this, rhs); }
  Vector3 Cross(const Vector3& rhs) const{ return Parent::Cross(*this, rhs); }
  
  static double Dot(const Vector3& lhs, const Vector3& rhs){ return Parent::Dot(lhs,rhs); }
  static Vector3 Cross(const Vector3& lhs, const Vector3& rhs){ return Parent::Cross(lhs,rhs); }
  template<typename T, typename U>
  static double Dot(const internal::BaseVec3<T>& lhs, const internal::BaseVec3<U>& rhs){
    return Parent::Dot((Vector3)lhs, (Vector3)rhs);
  }
  template<typename T, typename U>
  static Vector3 Cross(const internal::BaseVec3<T>& lhs, const internal::BaseVec3<U>& rhs){
    return Parent::Cross((Vector3)lhs, (Vector3)rhs);
  }

  static bool isPerpendicular(const Vector3& a,const Vector3& b){
    return fabs(Vector3::Dot(a,b)) < EPS;
  }

  static bool Same_Direction(const Vector3& a,const Vector3& b){
    if(a.Near_Zero() || b.Near_Zero()) return true;
    double t;
    for(int i=0;i<3;++i)
      if(b[i] != 0) { t = a[i]/b[i]; break; }
    return b*t == a;
  }
  
  static Vector3 Random_Unit(void);
  static Vector3 Random(const double& min, const double& max);

  Vector3 Transform(const Transformation&) const;
};


class Vector2 : public internal::BaseVec2<Vector2>{
private:
  using Parent = internal::BaseVec2<Vector2>;
public:
  using Parent::Parent;

  template<typename T>
  explicit Vector2(const BaseVec2<T>& rhs){ v[0]=rhs.v[0]; v[1]=rhs.v[1]; }

};



class Point2 : public internal::BaseVec2<Point2>{
private:
  using Parent = internal::BaseVec2<Point2>;
public:
  using Parent::Parent;

  template<typename T>
  explicit Point2(const BaseVec2<T>& rhs){ v[0]=rhs.v[0]; v[1]=rhs.v[1]; }

  Point2 operator + (const Point2&) = delete;
  Point2& operator += (const Point2&) = delete;
  Point2& operator -= (const Point2&) = delete;
  
  Point2 operator + (const Vector2& rhs) const;
  friend Point2 operator + (const Vector2& lhs, const Point2& rhs){ return rhs+lhs; }
  Point2& operator += (const Vector2& rhs) { return *this = *this + rhs; }
  Vector2 operator - (const Point2& rhs) const;
  Point2 operator - (const Vector2& rhs) const;
  Point2& operator -= (const Vector2& rhs){ return *this = *this - rhs; }

};
  

class Point3 : public internal::BaseVec3<Point3>{
private:
  using Parent = internal::BaseVec3<Point3>;
public:
  using Parent::Parent;

  template<typename T>
  explicit Point3(const BaseVec3<T>& rhs){ v[0]=rhs.v[0]; v[1]=rhs.v[1]; v[2]=rhs.v[2]; }

  Point3 operator + (const Point3&) = delete;
  Point3& operator += (const Point3&) = delete;
  Point3& operator -= (const Point3&) = delete;

  Point3 operator - () const { return Point3(-v[0], -v[1], -v[2]); }
  Point3 operator + (const Vector3& rhs) const { return Point3(v[0]+rhs[0], v[1]+rhs[1], v[2]+rhs[2]); }
  friend Point3 operator + (const Vector3& lhs, const Point3& rhs){ return rhs + lhs; }
  Point3& operator += (const Vector3& rhs) { return *this = *this + rhs; }
  Vector3 operator - (const Point3& rhs) const { return Vector3(v[0]-rhs[0], v[1]-rhs[1], v[2]-rhs[2]); }
  Point3 operator - (const Vector3& rhs) const { return *this + (-rhs); }
  Point3& operator -= (const Vector3& rhs){ return *this = *this - rhs; }

  Point3 Transform(const Transformation&) const;
};


class Normal : public internal::BaseVec3<Normal>{
private:
  using Parent = internal::BaseVec3<Normal>;
public:
  using Parent::Parent;

  template<typename T>
  explicit Normal(const BaseVec3<T>& rhs){ v[0]=rhs.v[0]; v[1]=rhs.v[1]; v[2]=rhs.v[2]; }

  Normal Transform(const Transformation&) const;
};
    


#endif
