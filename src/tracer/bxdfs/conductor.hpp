#pragma once

#include "bsdf.hpp"
#include "trowbridge_reitz_distribution.hpp"
#include "util/vector_fwd.hpp"

namespace bxdfs {

/**
 * @class Conductor
 * @brief Microfacet BRDF modeling metallic reflection.
 *
 * This class implements a conductor (metal) reflection model using a
 * Trowbridge--Reitz microfacet distribution.  The BRDF is of the form
 * \f[
 *   f_r(\omega_i,\omega_o) =
 *     D(h) \, G(\omega_i,\omega_o)
 *     \frac{|\omega_o\cdot h|}{4\,|\omega_i\cdot n|\,|\omega_o\cdot n|}
 *     \rho ,
 * \f]
 * where \f$h\f$ is the half vector and \f$\rho\f$ is the base color.
 */
class Conductor : public BxDF {
 public:
  /**
   * Construct a conductor BRDF.
   *
   * @param mfdist Microfacet distribution controlling roughness.
   * @param col    Surface reflectance color \f$\rho\f$.
   */
  explicit Conductor(TrowbridgeReitzDistribution mfdist, Color col);

  /**
   * Evaluate the BRDF value.
   *
   * @param wi Incident direction.
   * @param wo Outgoing direction.
   * @return BRDF value.
   */
  Color f(const Vector3& wi, const Vector3& wo) const override;

  /**
   * Sample an outgoing direction according to the microfacet distribution.
   */
  std::optional<bxdfSample> Sample_f(const Vector3& wi) const override;

  /**
   * PDF for sampling \c wo given \c wi.
   */
  double pdf(const Vector3& wi, const Vector3& wo) const override;

 private:
  TrowbridgeReitzDistribution mfdist_;  ///< Microfacet normal distribution
  double fuzz_;                         ///< Unused: compatibility only
  Color col_;                           ///< Surface color \f$\rho\f$
};

}  // namespace bxdfs
