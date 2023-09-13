#ifndef UTILITY_GEOMETRY_H
#define UTILITY_GEOMETRY_H

#include<cassert>
#include<format>
#include<iostream>

namespace develop{
  
  class Point3;
  class Normal;
  class Vector3{
  public:
    // constructors
    Vector3(){ v[0]=v[1]=v[2]=0; }
    Vector3(const double& e1,const double& e2,const double& e3){ v[0]=e1; v[1]=e2; v[2]=e3; }
    //Vector3(std::initializer_list<double> li){ v[0]=li[0]; v[1]=li[1]; v[2]=li[2]; }

    Vector3(Vector3&& mv){ v[0]=mv[0];v[1]=mv[1];v[2]=mv[2]; assert(isvalid()); }
    Vector3(const Vector3& cp){ v[0]=cp[0];v[1]=cp[1];v[2]=cp[2]; assert(isvalid()); }
    Vector3& operator = (const Vector3& cp){ v[0]=cp.v[0]; v[1]=cp.v[1]; v[2]=cp.v[2]; assert(isvalid()); return *this; }
    Vector3& operator = (Vector3&& mv){ v[0]=mv.v[0]; v[1]=mv.v[1]; v[2]=mv.v[2]; assert(isvalid()); return *this; }
  
    explicit Vector3(const Point3& p);
    explicit Vector3(const Normal& p);
  
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
    Vector3 operator -()  const { return Vector3{-v[0],-v[1],-v[2]}; }
    Vector3 operator +(const Vector3& rhs) const { return Vector3{v[0]+rhs[0], v[1]+rhs[1], v[2]+rhs[2]}; }
    Vector3& operator += (const Vector3& rhs){ return *this = *this + rhs; }
    Vector3 operator -(const Vector3& rhs) const { return *this + (-rhs); }
    Vector3& operator -= (const Vector3& rhs){ return *this = *this + (-rhs); }
    Vector3 operator *(const double& rhs) const { return Vector3{v[0]*rhs, v[1]*rhs, v[2]*rhs}; }
    Vector3& operator *=(const double& rhs){ return *this = *this * rhs; }
    Vector3 operator /(const double& rhs) const{ return Vector3{v[0]/rhs, v[1]/rhs, v[2]/rhs}; }
    Vector3& operator /=(const double& rhs){ return *this = *this / rhs; }
    bool operator == (const Vector3& rhs) const ;
    bool operator != (const Vector3& rhs) const ;
    bool Near_Zero() const;
    
    // debug
    bool hasnans(void) const;
    bool isvalid(void) const;

    static double Length(const Vector3&);
    static double Length_squared(const Vector3&);
    double Length() const;
    double Length_squared() const;

    static Vector3 Normalize(const Vector3& v);
    Vector3& Normalize();


    static Vector3 Random_Unit();
    static Vector3 Random(const double& min=-1.0, const double& max=1.0);

    static double Dot(const Vector3& u,const Vector3& v);
    static Vector3 Cross(const Vector3& u,const Vector3& v);
    double Dot(const Vector3&) const;
    Vector3& Cross(const Vector3&);

    double v[3];
  };
  inline std::ostream& operator << (std::ostream& os, const Vector3& v){
    os << std::format("({},{},{})", v[0],v[1],v[2]); return os;
  }
  inline Vector3 operator * (const double& lhs,const Vector3& rhs){ return rhs * lhs; }


  class Point2;
  class Vector2{
  public:
    Vector2(){ v[0] = v[1] = 0.0; }
    Vector2(const double& x, const double& y){ v[0]=x; v[1]=y; }
    //Vector2(std::initializer_list<double> li){ v[0]=li[0]; v[1]=li[1]; }
    
    Vector2(const Vector2& cp){ v[0]=cp.v[0]; v[1]=cp.v[1]; assert(isvalid()); }
    Vector2(Vector2&& mv){ v[0]=mv.v[0]; v[1]=mv.v[1]; assert(isvalid()); }
    Vector2& operator = (const Vector2& cp){ v[0]=cp.v[0]; v[1]=cp.v[1]; assert(isvalid()); return *this; }
    Vector2& operator = (Vector2&& mv){ v[0]=mv.v[0]; v[1]=mv.v[1]; assert(isvalid()); return *this; }
    
    // data access methods
    double const x() const  { return v[0]; }
    double& x()  { return v[0]; }
    double const y() const  { return v[1]; }
    double& y()  { return v[1]; }
    double operator [](const int& i) const{ return v[i]; }
    double& operator [](const int& i){ return v[i]; }

    // arithmetics
    Vector2 operator -()  const{ return Vector2{-v[0],-v[1]}; }
    Vector2 operator + (const Vector2& rhs)  const { return Vector2{v[0]+rhs[0],v[1]+rhs[1]}; }
    Vector2& operator += (const Vector2& rhs){ return *this = *this + rhs; }
    Vector2 operator - (const Vector2& rhs) const { return *this + (-rhs); }
    Vector2& operator -= (const Vector2& rhs){ return *this = *this - rhs; }
    Vector2 operator * (const double& rhs) const { return Vector2{v[0]*rhs,v[1]*rhs}; }
    Vector2& operator *= (const double& rhs){ return *this = *this * rhs; }
    Vector2 operator / (const double& rhs) const { return Vector2{v[0]/rhs, v[1]/rhs}; }
    Vector2& operator /= (const double& rhs){ return *this = *this / rhs; }
    bool operator == (const Vector2& rhs) const ;
    bool operator != (const Vector2& rhs) const ;

    bool hasnans(void) const;
    bool isvalid(void) const;
    
    bool Near_Zero() const;

    static double Length_squared(const Vector2& v);
    double Length_squared() const;
    static double Length(const Vector2& v);
    double Length() const;

    static Vector2 Normalize(const Vector2& v);
    Vector2& Normalize(void);
    
    static double Dot(const Vector2& lhs, const Vector2& rhs){ return lhs[0]*rhs[0]+lhs[1]*rhs[1]; }
    double Dot(const Vector2& rhs) const { return Vector2::Dot(*this, rhs); }
    static Vector3 Cross(const Vector2& lhs, const Vector2& rhs);
    Vector3 Cross(const Vector2& rhs) const;

  
    double v[2];
  };
  inline std::ostream& operator << (std::ostream& os, const Vector2& v){
    os << std::format("({},{})", v[0],v[1]); return os;
  }
  inline Vector2 operator * (const double& lhs,const Vector2& rhs){ return rhs * lhs; }


  class Point2{
  public:
    Point2(){ v[0]=v[1]=0; }
    Point2(const double& x,const double& y){ v[0]=x; v[1]=y; }
    //Point2(std::initializer_list<double> li){ v[0]=li[0]; v[1]=li[1]; }
    
    Point2(const Point2& cp){ v[0]=cp.v[0]; v[1]=cp.v[1]; assert(isvalid()); }
    Point2(Point2&& mv){ v[0]=mv.v[0]; v[1]=mv.v[1]; assert(isvalid()); }
    Point2& operator = (const Point2& cp){ v[0]=cp.v[0]; v[1]=cp.v[1]; assert(isvalid()); return *this; }
    Point2& operator = (Point2&& mv){ v[0]=mv.v[0]; v[1]=mv.v[1]; assert(isvalid()); return *this; }

    explicit Point2(const Point3& p);
    explicit Point2(const Vector2& p){ v[0]=p[0]; v[1]=p[1]; }

    double x() const  { return v[0]; }
    double& x()  { return v[0]; }
    double y() const { return v[1]; }
    double& y()  { return v[1]; }
    double operator [] (const int& idx) const { return v[idx]; }
    double& operator [] (const int& idx){ return v[idx]; }

    Point2 operator - () const { return Point2(-v[0], -v[1]); }
    Point2 operator + (const Vector2& rhs) const { return Point2(v[0]+rhs[0], v[1]+rhs[1]); }
    Point2& operator += (const Vector2& rhs) { return *this = *this + rhs; }
    Vector2 operator - (const Point2& rhs) const { return Vector2(v[0]-rhs[0], v[1]-rhs[1]); }
    Point2 operator - (const Vector2& rhs) const { return *this + (-rhs); }
    Point2& operator -= (const Vector2& rhs){ return *this = *this - rhs; }

    bool operator == (const Point2& rhs) const ;
    bool operator != (const Point2& rhs) const ;
    
    bool hasnans() const;
    bool isvalid() const;    

    
    double v[2];
  };
  inline std::ostream& operator << (std::ostream& os, const Point2& v){
    os << std::format("({},{})", v[0],v[1]); return os;
  }
  

  class Point3{
  public:
    Point3(){ v[0]=v[1]=v[2]=0; }
    Point3(const double& x,const double& y, const double& z){ v[0]=x; v[1]=y; v[2]=z; }
    //Point3(std::initializer_list<double> li){ v[0]=li[0]; v[1]=li[1]; v[2]=li[2]; }
    
    Point3(const Point3& cp){ v[0]=cp.v[0]; v[1]=cp.v[1]; v[2]=cp.v[2]; assert(isvalid()); }
    Point3(Point3&& mv){ v[0]=mv.v[0]; v[1]=mv.v[1]; v[2]=mv.v[2]; assert(isvalid()); }
    Point3& operator = (const Point3& cp){ v[0]=cp.v[0]; v[1]=cp.v[1]; v[2]=cp.v[2]; assert(isvalid()); return *this; }
    Point3& operator = (Point3&& mv){ v[0]=mv.v[0]; v[1]=mv.v[1]; v[2]=mv.v[2]; assert(isvalid()); return *this; }
    
    explicit Point3(const Point2& p){ v[0]=p.v[0]; v[1]=p.v[1];}
    explicit Point3(const Vector3& p){ v[0]=p[0]; v[1]=p[1]; v[2]=p.v[2]; }

    double x() const  { return v[0]; }
    double& x()  { return v[0]; }
    double y() const { return v[1]; }
    double& y()  { return v[1]; }
    double z() const { return v[2]; }
    double& z()  { return v[2]; }
    double operator [] (const int& idx) const { return v[idx]; }
    double& operator [] (const int& idx){ return v[idx]; }

    Point3 operator - () const { return Point3(-v[0], -v[1], -v[2]); }
    Point3 operator + (const Vector3& rhs) const { return Point3(v[0]+rhs[0], v[1]+rhs[1], v[2]+rhs[2]); }
    Point3& operator += (const Vector3& rhs) { return *this = *this + rhs; }
    Vector3 operator - (const Point3& rhs) const { return Vector3(v[0]-rhs[0], v[1]-rhs[1], v[2]-rhs[2]); }
    Point3 operator - (const Vector3& rhs) const { return *this + (-rhs); }
    Point3& operator -= (const Vector3& rhs){ return *this = *this - rhs; }

    bool operator == (const Point3& rhs) const ;
    bool operator != (const Point3& rhs) const ;
    
    bool hasnans() const;
    bool isvalid() const;    

    
    double v[3];
  };


  class Normal{
  public:
    Normal(){ v[0]=v[1]=v[2]=0; }
    Normal(const double& x,const double& y,const double& z){ v[0]=x; v[1]=y; v[2]=z; }
    //Normal(std::initializer_list<double> li){ v[0]=li[0]; v[1]=li[1]; v[2]=li[2]; }

    explicit Normal(const Vector3& p){ v[0]=p[0]; v[1]=p[1]; v[2]=p[2]; }
    explicit Normal(const Point3& p){ v[0]=p[0]; v[1]=p[1]; v[2]=p[2]; }

    double& x() { return v[0]; }
    double& y() { return v[1]; }
    double& z() { return v[2]; }
    double x()  const{ return v[0]; }
    double y()  const{ return v[1]; }
    double z()  const{ return v[2]; }
    double operator[] (const int& i) const{ return v[i]; }
    double& operator[] (const int& i){ return v[i]; }

    double v[3];
  };
    

} // namespace develop
#endif
