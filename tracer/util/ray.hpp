#ifndef RAY_H
#define RAY_H

#include "vector3.hpp"

#include<memory>

class Ray{
public:
  Ray(){}
  Ray(const Point3& origin_, const Vector3& direction_) : origin(origin_), direction(direction_) {}

  Point3 Origin() const{ return origin; }
  Vector3 Direction() const{ return direction; }

  Point3 At(const double&) const;

  double intersectionTimeWithPlane(const Point3&,const Vector3&,const Vector3&) const;
  
  static Vector3 Scatter_Direction(const Vector3& normal);
  static Vector3 Reflect_Direction(const Vector3& in_direction, const Vector3& normal);
  static Vector3 Refract_Direction(const Vector3& in_direction, const Vector3& normal, const double& refractive_index_ratio);
private:
  Point3 origin;
  Vector3 direction;
};


class Visible;
class Hit_record{		// Records with a ray hits a visible Shape object
public:
  Hit_record() = delete;
  static Hit_record MakeHitRecordWith_ORTPN(std::shared_ptr<Visible> _hitted_Obj,
					    const Ray& _Ray,
					    const double& _Time,
					    const Point3& _Pos,
					    const Vector3& _out_Normal){
    return Hit_record(_hitted_Obj, _Ray, _Time, _Pos, _out_Normal);
  }

  static Hit_record NoHit(void){ return Hit_record(nullptr); }

private:
  Hit_record(std::shared_ptr<Visible> _hitted_obj,
	     const Ray& _ray,
	     const double& _time,
	     const Point3& _pos,
	     const Vector3& _out_normal) :
    hits(true),hit_counts(1),
    hitted_obj(_hitted_obj),
    position(_pos),
    time(_time)
  {
    Set_Face_Normal(_ray, _out_normal);
  }
  Hit_record(void* _dummy) : //nohit
    hits(false) {}
  
public:
  bool hits;
  int hit_counts;
  std::shared_ptr<Visible> hitted_obj;
  Ray ray;
					    
  
  Point3 position;	        // hit position
  Vector3 normal;		// normal vector, at the same side with ray
  double time;			// time
  bool front_face;		// is this the front face of the hitted obj?

  void Set_Face_Normal(const Ray& r, const Vector3& outward_normal){
    front_face = Vector3::Dot(r.Direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
    ray = r;
  }
};

#endif
