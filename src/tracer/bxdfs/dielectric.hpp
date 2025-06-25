#pragma once

#include "bsdf.hpp"
#include "trowbridge_reitz_distribution.hpp"
#include "util/random.hpp"

namespace bxdfs {

/**
 * @class Dielectric
 * @brief Microfacet model for dielectric materials.
 *
 * Handles both reflection and transmission using a Fresnel term and the
 * Trowbridge--Reitz distribution.  For rough surfaces the BSDF combines
 * reflection and refraction according to
 * \f[
 *   f(\omega_i,\omega_o)=\frac{D(h)\,G(\omega_i,\omega_o)}
 *     {4 |\omega_i\cdot n| |\omega_o\cdot n|} F_r
 * \f]
 * for reflection and a similar term for transmission.
 */
class Dielectric : public BxDF {
 public:
  /**
   * Construct a dielectric BSDF.
   *
   * @param eta    Index of refraction of the material.
   * @param mfdist Microfacet distribution describing surface roughness.
   */
  explicit Dielectric(
      double eta,
      TrowbridgeReitzDistribution mfdist = TrowbridgeReitzDistribution(0, 0));

  /// Evaluate the BSDF value.
  Color f(const Vector3& wi, const Vector3& wo) const override;

  /// Importance sample an outgoing direction.
  std::optional<bxdfSample> Sample_f(const Vector3& wi) const override;

  /// PDF of sampling \c wo given \c wi.
  double pdf(const Vector3& wi, const Vector3& wo) const override;

 private:
  double eta;                           ///< Index of refraction
  TrowbridgeReitzDistribution mfdist_;  ///< Microfacet distribution
};

}  // namespace bxdfs
