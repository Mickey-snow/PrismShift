#pragma once

class ILight {
 public:
  virtual ~ILight() = default;

  virtual Hit_record Hit(const Ray&, const Interval<double>&) const = 0;
  virtual Color Emission(const Hit_record&) const = 0;
};
