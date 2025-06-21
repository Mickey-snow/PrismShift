#include "material.hpp"

#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "hit_record.hpp"
#include "texture.hpp"

#include "spdlog/spdlog.h"

using namespace bxdfs;

// ------------------------------------------------------------------------------
DiffuseMaterial::DiffuseMaterial(Color c) : DiffuseMaterial(make_texture(c)) {}
DiffuseMaterial::DiffuseMaterial(std::shared_ptr<ITexture<Color>> c)
    : albedo_(std::move(c)) {}

BSDF DiffuseMaterial::GetBSDF(const HitRecord& rec) const {
  return BSDF(
      std::make_shared<Lambertian>(albedo_->Evaluate(rec.uv)),
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}

// ------------------------------------------------------------------------------
ConductorMaterial::ConductorMaterial(Color color, double uRough, double vRough)
    : ConductorMaterial(make_texture(color),
                        make_texture(uRough),
                        make_texture(vRough)) {}
ConductorMaterial::ConductorMaterial(Texture<Color> c,
                                     Texture<double> ur,
                                     Texture<double> vr)
    : albedo_(std::move(c)),
      uRoughness_(std::move(ur)),
      vRoughness_(std::move(vr)) {}

BSDF ConductorMaterial::GetBSDF(const HitRecord& rec) const {
  Color col = albedo_->Evaluate(rec.uv);
  TrowbridgeReitzDistribution ggx(uRoughness_->Evaluate(rec.uv),
                                  vRoughness_->Evaluate(rec.uv));
  return BSDF(
      std::make_shared<Conductor>(std::move(ggx), col),
      QuaternionTransform::RotateFrTo(Vector3(rec.normal), Vector3(0, 1, 0)));
}

// ------------------------------------------------------------------------------
DielectricMaterial::DielectricMaterial(double eta, double uRough, double vRough)
    : DielectricMaterial(make_texture(eta),
                         make_texture(uRough),
                         make_texture(vRough)) {}
DielectricMaterial::DielectricMaterial(Texture<double> eta,
                                       Texture<double> uRough,
                                       Texture<double> vRough)
    : eta_(std::move(eta)),
      uRoughness_(std::move(uRough)),
      vRoughness_(std::move(vRough)) {}

BSDF DielectricMaterial::GetBSDF(const HitRecord& rec) const {
  double eta = eta_->Evaluate(rec.uv);
  TrowbridgeReitzDistribution ggx(uRoughness_->Evaluate(rec.uv),
                                  vRoughness_->Evaluate(rec.uv));
  return BSDF(
      std::make_shared<Dielectric>(eta, std::move(ggx)),
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
