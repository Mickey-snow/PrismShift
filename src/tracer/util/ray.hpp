#ifndef RAY_H
#define RAY_H

#include "geometry.hpp"

#include<memory>

class Transformation;

class Ray{
public:
  Ray(){}
  Ray(const Point3& origin_, const Vector3& direction_) : origin(origin_), direction(direction_) {}

  Point3 Origin() const{ return origin; }
  Vector3 Direction() const{ return direction; }
  Ray& SetOrigin(const Point3& orig){ origin=orig; return *this; }
  Ray& SetDirection(const Vector3& dir){ direction=dir; return *this; }

  Point3 At(const double&) const;

  [[deprecated]]
  double intersectionTimeWithPlane(const Point3&,const Vector3&,const Vector3&) const;
  
  Vector3 Scatter_Direction(const Normal& normal) const;
  Vector3 Reflect_Direction(const Normal& normal) const;
  Vector3 Refract_Direction(const Normal& normal, const double& refractive_index_ratio) const;

  Ray Transform(const Transformation&) const;
private:
  Point3 origin;
  Vector3 direction;
};


class Visible;
class Hit_record{		// Records with a ray hits a visible Shape object
public:
  Hit_record() = delete;
  static Hit_record MakeHitRecordWith_ORTPN(Visible const* _hitted_Obj,
					    const Ray& _Ray,
					    const double& _Time,
					    const Point3& _Pos,
					    const Normal& _out_Normal){
    return Hit_record(_hitted_Obj, _Ray, _Time, _Pos, _out_Normal);
  }

  static Hit_record NoHit(void){ return Hit_record(nullptr); }

private:
  Hit_record(Visible const* _hitted_obj,
	     const Ray& _ray,
	     const double& _time,
	     const Point3& _pos,
	     const Normal& _out_normal) :
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
  Visible const* hitted_obj;
  Ray ray;
					    
  
  Point3 position;	        // hit position
  Normal normal;		// normal vector, at the same side with ray
  double time;			// time
  bool front_face;		// is this the front face of the hitted obj?

  void Set_Face_Normal(const Ray& r, const Normal& outward_normal){
    front_face = Vector3::Dot(r.Direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
    ray = r;
  }

  [[deprecated("remove this method later")]]
  Hit_record _ToWorld() const;
};

#endif
