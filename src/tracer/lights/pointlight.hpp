#pragma once

#include <bsdf.hpp>
#include <factory.hpp>
#include <material.hpp>
#include <util/util.hpp>

#include <sstream>
#include <string>

class Pointlight : public Material {
 public:
  static constexpr std::string name{"pointlight"};

  Pointlight();

  Pointlight& Set_Color(const double&, const double&, const double&);
  Pointlight& Set_Color(const Color&);

  virtual BSDF CalculateBSDF(const Hit_record&) override { return {}; }

  Color Emission(const Hit_record&) override { return color; }

 private:
  Color color;
};

namespace {
std::shared_ptr<Material> CreatePointlight(
    const std::vector<Attribute>& attributes) {
  auto ret = std::make_shared<Pointlight>();

  for (const auto& attr : attributes)
    if (attr.name == "rgb") {
      try {
        auto rgb_arr = std::any_cast<std::array<double, 3>>(attr.val);
        ret->Set_Color(rgb_arr[0], rgb_arr[1], rgb_arr[2]);
        continue;
      } catch (std::bad_cast& e) {
      }
      try {
        auto col = std::any_cast<Color>(attr.val);
        ret->Set_Color(col);
        continue;
      } catch (std::bad_cast& e) {
      }
      throw std::runtime_error(
          "at CreatePointlight: value type for attribute rgb is unknown");
    }

  return ret;
}
constexpr std::string Pointlight_MaterialID = Pointlight::name;
const bool pointlight_registered =
    MaterialFactory::Instance()->Register(Pointlight_MaterialID,
                                          CreatePointlight);
}  // namespace
