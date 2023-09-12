#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include<util/util.hpp>
#include<material.hpp>
#include<factory.hpp>
#include<texture.hpp>

#include<string>
#include<memory>

class Dielectric : public Material{
public:
  static constexpr std::string name{"dielectric"};

  Dielectric(const double& index_of_refraction) : eta(index_of_refraction) {}

  virtual std::vector<std::shared_ptr<BxDF>> CalculateBSDF(const Hit_record&) override;

private:
  double eta;
};

namespace{
}



#endif
