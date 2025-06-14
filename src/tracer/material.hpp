#pragma once

#include <memory>
#include <util/color.hpp>

class BSDF;
class HitRecord;
template <typename T>
class Texture;

struct Material {
  std::shared_ptr<Texture<Color>> diffuse = nullptr;
  std::shared_ptr<Texture<double>> dielectric = nullptr;
};
