#include "coordinate.hpp"
#include "transform.hpp"
#include "geometry.hpp"

Coordinate3& Coordinate3::Set_Translate(const double& x, const double& y, const double& z){
  return Set_Translate(Transformation::Translate(x, y, z));
}

Coordinate3& Coordinate3::Set_Rotate(const double& x, const double& y, const double& z){
  return Set_Rotate(Transformation::RotateX(x) *
		Transformation::RotateY(y) *
		Transformation::RotateZ(z));
}

Coordinate3& Coordinate3::Set_Scale(const double& x, const double& y, const double& z){
  return Set_Scale(Transformation::Scale(Vector3{x,y,z}));
}
