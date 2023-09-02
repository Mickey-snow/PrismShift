#ifndef OBJECTS_H
#define OBJECTS_H

#include "util/util.hpp"
#include "material.hpp"

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

class Visible : public Shape{
public:
  virtual ~Visible() = default;

  virtual void Set_material(std::shared_ptr<Material>) = 0;

  // Return the color of the Ray r that hits an object
  virtual Color Ray_Color(const Ray& R, const Hit_record& rec) const = 0;

  virtual Point2 Map_Texture(const Ray&, const Hit_record&) const = 0;

};
#endif
