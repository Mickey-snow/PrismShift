#ifndef OBJECTS_H
#define OBJECTS_H

#include "util/util.hpp"

#include<memory>
#include<string>


class AABB;
class Shape{
public:
  virtual ~Shape() = default;

  virtual std::string Get_Name(void) const = 0;
  
  // An alternative Hit method
  // Accepts Ray r and Interval time as parameters
  // Returns a Hit_record encapuslates the first object Ray r hits.
  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const = 0;

  virtual AABB Get_Bounding_box(void) const = 0;
};

class Material;
class Visible : public Shape{
public:
  virtual ~Visible() = default;

  virtual void Set_Material(std::shared_ptr<Material>) = 0;
  virtual std::shared_ptr<Material> Get_Material(void) = 0;
  
  virtual Point2 Map_Texture(const Hit_record&) const = 0;
};


#endif
