#ifndef COORDINATE_H
#define COORDINATE_H

#include<utility>

#include "util/transform.hpp"
#include "util/geometry_fwd.hpp"


class Coordinate3{
public:
  Coordinate3() = default;

  Coordinate3& Set_Translation(const double& x,const double& y,const double& z);
  Coordinate3& Set_Rotation(const double& x,const double& y, const double& z);
  Coordinate3& Set_Scale(const double& x,const double& y, const double& z);

  Coordinate3& Set_Translation(const MatrixTransformation& it){ translation = it; Update(); return *this;}
  Coordinate3& Set_Rotation(const MatrixTransformation& it){ rotation = it; Update(); return *this; }
  Coordinate3& Set_Scale(const MatrixTransformation& it){ scale = it; Update(); return *this; }

  template<typename T>
  T Local2World(const T& it) const{
    return toworld(it);
  }
  template<typename T>
  T World2Local(const T& it) const{
    return (toworld.Inverse())(it);
  }
  
private:
  MatrixTransformation translation,rotation,scale;
  MatrixTransformation toworld;

  void Update(){ toworld = translation * rotation * scale; }


public:
  static MatrixTransformation AlignXYZ(const Vector3&, const Vector3&, const Vector3&);
  static MatrixTransformation AlignXY(const Vector3&, const Vector3&);
  static MatrixTransformation AlignYZ(const Vector3&, const Vector3&);
  static MatrixTransformation AlignXZ(const Vector3&, const Vector3&);
  static MatrixTransformation Origin(const Point3&);


  
public:
  Coordinate3(const Coordinate3& rhs) : translation(rhs.translation), rotation(rhs.rotation), scale(rhs.scale) { Update(); }
  
  Coordinate3& operator = (const Coordinate3& rhs){
    translation = rhs.translation;
    rotation = rhs.rotation;
    scale = rhs.scale;
    Update();
    return *this;
  }
  
  Coordinate3(Coordinate3&& rhs) : translation(std::move(rhs.translation)), rotation(std::move(rhs.rotation)), scale(std::move(rhs.scale)) { Update(); }
  
  Coordinate3& operator = (Coordinate3&& rhs){
    translation = std::move(rhs.translation);
    rotation = std::move(rhs.rotation);
    scale = std::move(rhs.scale);
    Update();
    return *this;
  }
};


#endif
