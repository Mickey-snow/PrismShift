#ifndef MATERIAL_H
#define MATERIAL_H

#include "util/util.hpp"

class MockedBSDF{
public:
  virtual bool doScatter() = 0;
  virtual Vector3 ScatterDirection() = 0;
  virtual Color Emission() = 0;
  virtual Color f() = 0;
};

class Material{
public:
  ~Material() = default;

  virtual std::shared_ptr<MockedBSDF> CalculateBSDF(const Hit_record&) = 0;
};


#endif
