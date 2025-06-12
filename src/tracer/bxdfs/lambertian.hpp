#pragma once

#include <bsdf.hpp>

namespace bxdfs {

/**
 * @class Lambertian
 * @brief Implements the Lambertian BRDF for perfectly diffuse reflection.
 *
 * The Lambertian BRDF is defined as:
 * \f[
 *   f(\omega_i, \omega_o) = \frac{\rho}{\pi}
 *     \quad (\text{if } \cos\theta_i \cos\theta_o > 0,\;\text{0 otherwise})
 * \f]
 * where \f$\rho\f$ is the surface albedo (color), and \f$\theta_i\f$,
 * \f$\theta_o\f$ are the angles between the incident/outgoing directions and
 * the surface normal.
 *
 */
class Lambertian : public BxDF {
 public:
  /**
   * @brief Construct a Lambertian BRDF with given albedo.
   * @param _col Surface albedo color \f$\rho\f$.
   */
  explicit Lambertian(Color _col);

  /**
   * @brief Evaluate the BRDF for given incoming and outgoing directions.
   *
   * Returns \f$\rho/\pi\f$ if both \c wi and \c wo lie in the same hemisphere
   * around the surface normal, otherwise returns zero.
   *
   * Equation:
   * \f[
   *   f(\omega_i, \omega_o) =
   *     \begin{cases}
   *       \frac{\rho}{\pi}, & \text{if } \cos\theta_i \cos\theta_o > 0, \\
   *       0, & \text{otherwise}.
   *     \end{cases}
   * \f]
   *
   * @param wi  Incident direction (unit vector).
   * @param wo  Outgoing direction (unit vector).
   * @return BRDF value as RGB color.
   */
  Color f(const Vector3& wi, const Vector3& wo) const override;

  /**
   * @brief Sample an outgoing direction \c wo given an incoming direction \c
   * wi.
   *
   * Samples \c wo from a cosine-weighted distribution over the hemisphere
   * around the normal (aligned with +Y), then flips it if \c wi is below the
   * surface.
   *
   * Equation for PDF:
   * \f[
   *   p(\omega_o) = \frac{\cos\theta_o}{\pi}.
   * \f]
   *
   * @param wi  Incident direction (unit vector).
   * @return A \c bxdfSample containing:
   *   - \c f: the BRDF value \f$\rho/\pi\f$;
   *   - \c wo: sampled outgoing direction;
   *   - \c pdf: probability density of \c wo;
   *   - \c bxdf: pointer to this BRDF instance.
   */
  std::optional<bxdfSample> Sample_f(const Vector3& wi) const override;

  /**
   * @brief Compute the PDF of sampling \c wo given \c wi.
   *
   * Returns \f$\cos\theta_o / \pi\f$ if \c wi and \c wo are in the same
   * hemisphere, zero otherwise.
   *
   * @param wi  Incident direction (unit vector).
   * @param wo  Outgoing direction (unit vector).
   * @return PDF value.
   */
  double pdf(const Vector3& wi, const Vector3& wo) const override;

 private:
  Color col;  ///< Surface albedo color \f$\rho\f$.
};

}  // namespace bxdfs
