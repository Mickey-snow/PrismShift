#pragma once

#include <memory>

#include <shape.hpp>

class IMaterial;
class Hit_record;
template <typename>
class Interval;
class Ray;
class BSDF;
class AABB;

class IPrimitive : public IShape {
 public:
  virtual ~IPrimitive() = default;

  // Hit_record Hit(const Ray&, const Interval<double>&) const
  // AABB Get_Bbox(void) const
  // methods for geometry calculation

  virtual BSDF CalcBSDF(const Hit_record&) const = 0;
  // methods for calculate scattering distribution
};

class Primitive : public IPrimitive {
 public:
  Primitive();
  ~Primitive();

  Hit_record Hit(const Ray& r, const Interval<double>& t) const override;
  AABB Get_Bbox(void) const override;
  Primitive& Set_Shape(std::shared_ptr<IShape> shape) {
    m_shape = shape;
    return *this;
  }

  BSDF CalcBSDF(const Hit_record&) const override;
  Primitive& Set_Material(std::shared_ptr<IMaterial> material) {
    m_material = material;
    return *this;
  }

 private:
  std::shared_ptr<IMaterial> m_material;
  std::shared_ptr<IShape> m_shape;
};
