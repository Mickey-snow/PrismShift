#ifndef OBJECTS_H
#define OBJECTS_H

#include<util/geometry.hpp>
#include<util/ray.hpp>
#include<util/transform.hpp>
#include<util/aabb.hpp>

#include<memory>
#include<string>
#include<optional>

class AABB;
class Ray;
struct Hit_record;
template<typename T>class Interval;
class Point2;
class IPrimitive;

class IShape{
public:
  virtual ~IShape() = default;

  virtual Hit_record Hit(const Ray&, const Interval<double>&) const = 0;
  virtual AABB Get_Bbox(void) const = 0;
};


class ConcreteShape : public IShape{
public:
  ConcreteShape();
  ~ConcreteShape();

  virtual Hit_record Hit(const Ray&, const Interval<double>&) const override;
  virtual AABB Get_Bbox(void) const override;

  ConcreteShape& Set_Shape(IShape const *shape){
    m_shape = shape;
    m_bbox_rec.reset();
    return *this;
  }
  ConcreteShape& Set_Frame(const Transformation& frame){
    m_frame = frame;
    m_bbox_rec.reset();
    return *this;
  }

private:
  IShape const *m_shape;
  Transformation m_frame;
  mutable std::optional<AABB> m_bbox_rec;
};


struct Hit_record{
  Hit_record() : hits(false), hitted_obj(nullptr) {}
  Hit_record(const Ray& r, const double& t, const Normal& n) :
    hits(true), hitted_obj(nullptr),time(t),position(r.At(t)){ Set_Face_Normal(r,n); }


  bool hits;
  IPrimitive const* hitted_obj;
  double time;
  Point3 position;
  
  Ray ray;
  Normal normal;		// normal is at the same side with ray
  bool front_face;

  
  void Set_Face_Normal(const Ray& r, const Normal& outward_normal){
    front_face = Vector3::Dot(r.Direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
    ray = r;
  }

  bool isHit(void) const noexcept{ return hits; }
  
  template<typename... Ts>
  static Hit_record RTN(Ts&&... params){
    static_assert(sizeof...(Ts) > 0);
    return Hit_record(std::forward<Ts>(params)...);
  }

  template<typename... Ts>
  static Hit_record ORTN(IPrimitive const* _hitted_obj,
			 Ts&&... params){
    auto rec = RTN(std::forward<Ts>(params)...);
    rec.hitted_obj = _hitted_obj;
    return rec;
  }
};

#endif
