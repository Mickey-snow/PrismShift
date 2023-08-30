#include<iostream>
#include<cmath>

#include "vector3.hpp"
#include "random.hpp"
using std::sqrt;

Vector3::Vector3():e{0,0,0} {}
Vector3::Vector3(const double& e0,const double& e1,const double& e2):e{e0,e1,e2} {}
//Vector3::Vector3(std::initializer_list<double> ie):e{ie} {}

double Vector3::x() const{ return e[0]; }
double Vector3::y() const{ return e[1]; }
double Vector3::z() const{ return e[2]; }

bool Vector3::Near_Zero() const{
  static double EPS = 1e-8;
  return (fabs(e[0])<EPS) and (fabs(e[1])<EPS) and (fabs(e[2])<EPS);
}

Vector3 Vector3::operator - () const{
  return Vector3(-e[0],-e[1],-e[2]);
}
Vector3 Vector3::operator - (const Vector3& v) const{
  return Vector3(e[0]-v[0],e[1]-v[1],e[2]-v[2]);
}
Vector3& Vector3::operator -= (const Vector3& v){ return *this = *this-v; }
Vector3 Vector3::operator + (const Vector3& v) const{
  return Vector3(e[0]+v[0],e[1]+v[1],e[2]+v[2]);
}
Vector3& Vector3::operator += (const Vector3& v){ return *this = *this + v; }
Vector3 operator * (const Vector3& e, const double& t){
  return Vector3(t*e[0], t*e[1], t*e[2]);
}
Vector3 operator * (const double& t, const Vector3& e){ return e*t; }
Vector3& Vector3::operator *= (const double& t){ return *this = *this * t; }


Vector3 Vector3::operator / (const double& t)const{
  return Vector3(e[0]/t, e[1]/t, e[2]/t);
}
Vector3& Vector3::operator /= (const double& t){ return *this = *this / t; }

double Vector3::operator [](const int& i) const{ return e[i]; }
double& Vector3::operator [](const int& i){ return e[i]; }

double Vector3::Length() const{ return sqrt(this->Length_squared()); }
double Vector3::Length_squared() const{
  return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
}

std::ostream& operator <<(std::ostream& out, const Vector3& v){
  return out<<'('<<v[0]<<','<<v[1]<<','<<v[2]<<')';
}


double Dot(const Vector3 &u, const Vector3 &v) {
  return u[0] * v[0]
    + u[1] * v[1]
    + u[2] * v[2];
}

Vector3 Cross(const Vector3 &u, const Vector3 &v) {
  return Vector3(u[1] * v[2] - u[2] * v[1],
		 u[2] * v[0] - u[0] * v[2],
		 u[0] * v[1] - u[1] * v[0]);
}

Vector3 Unit_vector(const Vector3& v) {
  return v / v.Length();
}

Vector3 Vector3::Random_Unit(){
  Vector3 rand_v;
  do{
    rand_v = Vector3(random_double(-1.0,1.0), random_double(-1.0,1.0), random_double(-1.0,1.0));
  }while(false);
  return Unit_vector(rand_v);
}
