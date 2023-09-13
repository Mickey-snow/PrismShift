#include "transform.hpp"

#include<util/geometry.hpp>

using namespace develop;

Point3 Transform::operator() (const Point3& p) const{
  double x=p.x(),y=p.y(),z=p.z();
  double xp = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
  double yp = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
  double zp = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
  double wp = m[3][0]*x + m[3][1]*y + m[3][2]*z + m[3][3];
  return Point3(xp,yp,zp) / wp;
}

Vector3 Transform::operator() (const Vector3& p) const{
  double x=p.x(),y=p.y(),z=p.z();
  return Vector3(m[0][0]*x + m[0][1]*y + m[0][2]*z,
		 m[1][0]*x + m[1][1]*y + m[1][2]*z,
		 m[2][0]*x + m[2][1]*y + m[2][2]*z);
		 
}

Normal Transform::operator() (const Normal& p) const{
  double x=p.x(),y=p.y(),z=p.z();
  double xp = minv[0][0]*x + minv[1][0]*y + minv[2][0]*z + minv[3][0];
  double yp = minv[0][1]*x + minv[1][1]*y + minv[2][1]*z + minv[3][1];
  double zp = minv[0][2]*x + minv[1][2]*y + minv[2][2]*z + minv[3][2];
  double wp = minv[0][3]*x + minv[1][3]*y + minv[2][3]*z + minv[3][3];
  return Normal(xp,yp,zp) / wp;
}
