#ifndef OBJECTS_H
#define OBJECTS_H

#include<util/coordinate.hpp>

#include<memory>
#include<string>


class AABB;
class Ray;
class Hit_record;
template<typename T>class Interval;
class Point2;

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
  Visible() = default;
  Visible(const Coordinate3& local_reference_frame) : refframe(local_reference_frame) {}
  virtual ~Visible() = default;

  virtual void Set_Material(std::shared_ptr<Material>) = 0;
  virtual std::shared_ptr<Material> Get_Material(void) const = 0;
  
  virtual Point2 Map_Texture(const Hit_record&) const = 0;

// protected:
  Coordinate3 refframe;
};


#endif
