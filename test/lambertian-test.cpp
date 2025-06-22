#include <gtest/gtest.h>

#include "bsdf.hpp"
#include "bxdfs/lambertian.hpp"
#include "util/util.hpp"

#include <boost/math/distributions/kolmogorov_smirnov.hpp>
#include <cmath>

using namespace vec_helpers;
using namespace bxdfs;

static constexpr auto kEps = 1e-6;
namespace {  // helpers
inline static Vector3 rand_wi() {
  Vector3 v = rand_hemisphere_uniform();
  v.y() *= -1;
  return v;
}
}  // namespace

TEST(LambertianTest, F_ReturnsConstantColour) {
  const Color red{1.0, 0.0, 0.0};
  Lambertian lambert(red);

  Color val = lambert.f(/*wi*/ {0, -1, 1}, /*wo*/ {0, 1, 1});
  EXPECT_EQ(val, red * invpi);
  val = lambert.f({0, 11, -1}, {0, 1, 1});
  EXPECT_EQ(val, Color());
}

TEST(LambertianTest, SampleF_ProvidesSelfConsistentSample) {
  const Color c{0.2, 0.6, 0.8};
  Lambertian lambert(c);

  Vector3 wi{0, -1, 0};  // normal incidence
  auto sampleOpt = lambert.Sample_f(wi);
  ASSERT_TRUE(sampleOpt.has_value());

  const auto& s = *sampleOpt;
  EXPECT_EQ(s.f, c * invpi);

  // PDF in the sample must match calling pdf() directly
  EXPECT_NEAR(s.pdf, lambert.pdf(wi, s.wo), kEps);

  // All cosine–hemisphere samples must point into the +y half–space
  EXPECT_GT(s.wo.y(), 0.0);
}

TEST(LambertianTest, SampleF_IsCosineWeighted) {
  static constexpr size_t N = 2000;
  static const Normal n(0, 1, 0);
  Lambertian lambert(Color(1));

  std::vector<Float> cos_thetas(N, 0);
  for (size_t i = 0; i < N; ++i) {
    const auto wi = rand_wi();
    auto sample = lambert.Sample_f(wi);
    ASSERT_TRUE(sample.has_value());
    EXPECT_NEAR(sample->wo.Length_squared(), 1, kEps);
    EXPECT_GE(sample->wo.Dot(n), 0);

    cos_thetas[i] = sample->wo.Dot(n);
  }
  std::sort(cos_thetas.begin(), cos_thetas.end());

  // Compute the KS statistic D_n = max|F_emp(z) - F_theo(z)|,
  // where F_theo(z) = z^2 for z in [0,1].
  Float D = 0;
  for (size_t i = 0; i < N; ++i) {
    Float z = cos_thetas[i];
    Float F_emp1 = (i + 1.0) / N;               // empirical CDF just above z
    Float F_emp0 = static_cast<Float>(i) / N;  // empirical CDF just below z
    Float F_theo = z * z;                       // theoretical CDF

    D = std::max(D, std::abs(F_emp1 - F_theo));
    D = std::max(D, std::abs(F_emp0 - F_theo));
  }

  // Convert to the limiting statistic sqrt(n) * D_n
  Float stat = std::sqrt(N) * D;
  boost::math::kolmogorov_smirnov_distribution<> kolDist(N);
  Float K_cdf = cdf(kolDist, stat);

  Float p_value = 1.0 - K_cdf;  // p-value = 1 - K(stat)
  EXPECT_LT(p_value, 0.01) << "p_value = " << p_value;
}

TEST(LambertianTest, SampleConvergence) {
  // Compare two Monte Carlo estimators of integral
  // I = \int Li(w) cos(\theta) dw
  // Let Li(w)=1, the exact value shoule be pi.
  static constexpr auto N = 64;
  Lambertian lambert(Color(1));

  Float Iu = 0, Icos = 0;
  for (size_t i = 0; i < N; ++i) {
    auto wi = rand_wi();
    auto sample = lambert.Sample_f(wi);
    Icos += sample->wo.y() / sample->pdf;

    auto wo = rand_wi();
    Iu += wo.y() * 2 * pi;
  }

  Iu /= N;
  Icos /= N;
  Float err_iu = std::fabs(Iu - pi), err_icos = std::fabs(Icos - pi);
  EXPECT_LT(err_icos, err_iu) << "errors are: " << err_icos << " vs " << err_iu;
}
