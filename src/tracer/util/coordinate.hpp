#ifndef COORDINATE_H
#define COORDINATE_H

#include<utility>

#include "transform.hpp"

class Coordinate3{
public:
  Coordinate3() = default;

  Coordinate3& Set_Translate(const double& x,const double& y,const double& z);
  Coordinate3& Set_Rotate(const double& x,const double& y, const double& z);
  Coordinate3& Set_Scale(const double& x,const double& y, const double& z);

  Coordinate3& Set_Translate(const Transformation& it){ translation = it; Update(); return *this;}
  Coordinate3& Set_Rotate(const Transformation& it){ rotation = it; Update(); return *this; }
  Coordinate3& Set_Scale(const Transformation& it){ scale = it; Update(); return *this; }

  template<typename T>
  T Local2World(const T& it) const{
    return toworld(it);
  }
  template<typename T>
  T World2Local(const T& it) const{
    return (toworld.Inverse())(it);
  }
  
private:
  Transformation translation,rotation,scale;
  Transformation toworld;

  void Update(){ toworld = translation * rotation * scale; }
};


#endif
