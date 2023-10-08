#ifndef RAY_H
#define RAY_H

#include "geometry.hpp"

#include<memory>
#include<ostream>

class Transformation;

class Ray{
public:
  Ray(){}
  Ray(const Point3& origin_, const Vector3& direction_) : origin(origin_), direction(direction_) {}

  Point3 const Origin() const{ return origin; }
  Vector3 const Direction() const{ return direction; }
  Ray& SetOrigin(const Point3& orig){ origin=orig; return *this; }
  Ray& SetDirection(const Vector3& dir){ direction=dir; return *this; }

  Point3 At(const double&) const;

  Ray Transform(const Transformation&) const;

  bool operator == (const Ray& rhs) const {
    return origin==rhs.Origin() && direction==rhs.Direction();
  }
  bool operator != (const Ray& rhs) const { return !(*this==rhs); }
  
  friend std::ostream& operator << (std::ostream& os, const Ray& r){
    os << "O"<<r.Origin()<<" d"<<r.Direction();
    return os;
  }
  
private:
  Point3 origin;
  Vector3 direction;
};

#endif
