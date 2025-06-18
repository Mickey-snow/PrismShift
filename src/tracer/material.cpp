#include "material.hpp"

#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "hit_record.hpp"

#include "spdlog/spdlog.h"

using namespace bxdfs;

// ------------------------------------------------------------------------------
DiffuseMaterial::DiffuseMaterial(Color c) : color(c) {}

BSDF DiffuseMaterial::GetBSDF(const HitRecord& rec) const {
  return BSDF(
      std::make_shared<Lambertian>(color),
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}

// ------------------------------------------------------------------------------
ConductorMaterial::ConductorMaterial(Color color, double uRough, double vRough)
    : color_(color), uRoughness_(uRough), vRoughness_(vRough) {}

BSDF ConductorMaterial::GetBSDF(const HitRecord& rec) const {
  return BSDF(
      std::make_shared<Conductor>(
          TrowbridgeReitzDistribution(uRoughness_, vRoughness_), color_),
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}

// ------------------------------------------------------------------------------
DielectricMaterial::DielectricMaterial(double eta, double uRough, double vRough)
    : eta_(eta), uRoughness_(uRough), vRoughness_(vRough) {}

BSDF DielectricMaterial::GetBSDF(const HitRecord& rec) const {
  return BSDF(
      std::make_shared<Dielectric>(
          eta_, TrowbridgeReitzDistribution(uRoughness_, vRoughness_)),
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}

// ------------------------------------------------------------------------------
MixedMaterial::MixedMaterial(std::shared_ptr<IMaterial> first,
                             std::shared_ptr<IMaterial> second,
                             double fac)
    : first_(std::move(first)), second_(std::move(second)), fac_(fac) {}

std::shared_ptr<IMaterial> MixedMaterial::Select() const {
  if (random_uniform_01() < fac_)
    return first_;
  else
    return second_;
}

BSDF MixedMaterial::GetBSDF(const HitRecord&) const {
  spdlog::error("MixedMaterial::GetBSDF should not be called.");
  return BSDF(nullptr, QuaternionTransform(Quaternion(1, 0, 0, 0)));
}
