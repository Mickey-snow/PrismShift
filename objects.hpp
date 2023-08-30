#ifndef OBJECTS_H
#define OBJECTS_H

#include "vector3.hpp"
#include "ray.hpp"
#include "interval.hpp"
#include "color.hpp"

class Visible;

struct Hit_record{
  bool hits;
  Visible* hitted_obj;
  
  Point3 position;	        // hit position
  Vector3 normal;		// outward normal vector
  double time;			// time
  bool front_face;		// is this the front face of the hitted obj?

  void Set_Face_Normal(const Ray& r, const Vector3& outward_normal){
    front_face = Dot(r.Direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};


class Visible{
public:
  virtual ~Visible() = default;


  // returns true iff Ray r hits the caller object
  // within time interval [time.begin, time.end]
  // records the exact time rec.t:double and hitted position rec.p:Vector3 in Hit_record rec
  // also sets rec.normal:Vector3 as the outward normal vector of the hitted surface
  // also sets rec.front_face:bool to true iff Ray r hits the front face of the object.
  // too many responsibilities, needs a redesign
  // virtual bool Hit(const Ray& r, Interval time, Hit_record& rec)const = 0; 

  // An alternative Hit method
  // Accepts Ray r and Interval time as parameters
  // Returns a Hit_record encapuslates the first object Ray r hits.
  virtual Hit_record Ray_Hit(const Ray&r , const Interval& time) const = 0;

  // Return the color of the Ray r that hits an object
  virtual Color Ray_color(const Ray& r, const Hit_record& rec) const = 0;
};
  
#endif
