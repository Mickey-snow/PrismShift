#include "material.hpp"

#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "hit_record.hpp"

using namespace bxdfs;

// ------------------------------------------------------------------------------
DiffuseMaterial::DiffuseMaterial(Color c) : color(c) {}

BSDF DiffuseMaterial::GetBSDF(const HitRecord& rec) const {
  return BSDF(
      std::make_shared<Lambertian>(color),
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}

// ------------------------------------------------------------------------------
ConductorMaterial::ConductorMaterial(Color color, double fuzz)
    : color_(color), fuzz_(fuzz) {}

BSDF ConductorMaterial::GetBSDF(const HitRecord& rec) const {
  return BSDF(
      std::make_shared<Conductor>(
          TrowbridgeReitzDistribution(uRoughness_, vRoughness_), color_, fuzz_),
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}

// ------------------------------------------------------------------------------
DielectricMaterial::DielectricMaterial(double eta) : eta_(eta) {}

BSDF DielectricMaterial::GetBSDF(const HitRecord& rec) const {
  return BSDF(
      std::make_shared<Dielectric>(eta_),  // TODO: Rough Dielectric
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}
