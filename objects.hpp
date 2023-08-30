#ifndef OBJECTS_H
#define OBJECTS_H

#include "common/common.hpp"

class Visible;

class Hit_record{
public:
  bool hits;
  int hit_counts;
  std::shared_ptr<Visible> hitted_obj;
  
  Point3 position;	        // hit position
  Vector3 normal;		// outward normal vector
  double time;			// time
  bool front_face;		// is this the front face of the hitted obj?

  Hit_record():hits(false),hit_counts(0) {}

  void Set_Face_Normal(const Ray& r, const Vector3& outward_normal){
    front_face = Dot(r.Direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};


class Visible{
public:
  virtual ~Visible() = default;

  // An alternative Hit method
  // Accepts Ray r and Interval time as parameters
  // Returns a Hit_record encapuslates the first object Ray r hits.
  virtual Hit_record Ray_Hit(const Ray& r, const Interval<double>& time) const = 0;

  // Return the color of the Ray r that hits an object
  virtual Color Ray_Color(const Ray& R, const Hit_record& rec) const = 0;
};
  
#endif
