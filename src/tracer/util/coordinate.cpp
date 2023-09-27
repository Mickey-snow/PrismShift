#include "coordinate.hpp"
#include "transform.hpp"
#include "geometry.hpp"

Coordinate3& Coordinate3::Set_Translation(const double& x, const double& y, const double& z){
  return Set_Translation(Transformation::Translate(x, y, z));
}

Coordinate3& Coordinate3::Set_Rotation(const double& x, const double& y, const double& z){
  return Set_Rotation(Transformation::RotateX(x) *
		Transformation::RotateY(y) *
		Transformation::RotateZ(z));
}

Coordinate3& Coordinate3::Set_Scale(const double& x, const double& y, const double& z){
  return Set_Scale(Transformation::Scale(Vector3{x,y,z}));
}


Transformation Coordinate3::AlignXYZ(const Vector3& i, const Vector3& j, const Vector3& k){
  Matrix4 mat_rotate_toworld{
    i.x(), j.x(), k.x(), 0,
    i.y(), j.y(), k.y(), 0,
    i.z(), j.z(), k.z(), 0,
    0,0,0,1};

  return Transformation(mat_rotate_toworld);
}
Transformation Coordinate3::AlignXY(const Vector3& i,const Vector3& j){
  Vector3 k = Vector3::Cross(i,j);
  return Coordinate3::AlignXYZ(i,j,k);
}
Transformation Coordinate3::AlignXZ(const Vector3& i,const Vector3& k){
  Vector3 j = Vector3::Cross(k,i);
  return Coordinate3::AlignXYZ(i,j,k);
}
Transformation Coordinate3::AlignYZ(const Vector3& j,const Vector3& k){
  Vector3 i = Vector3::Cross(j,k);
  return Coordinate3::AlignXYZ(i,j,k);
}
  
Transformation Coordinate3::Origin(const Point3& o){
  return Transformation::Translate((Vector3)o);
}

