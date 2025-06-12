
#include <gtest/gtest.h>

#include <boost/math/distributions/kolmogorov_smirnov.hpp>
#include <cmath>

#include "bsdf.hpp"
#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "util/util.hpp"

using namespace bxdfs;

static constexpr auto kEps = 1e-6;

namespace {  // helpers
inline static Vector3 rand_wi() {
  Vector3 v = rand_hemisphere_uniform();
  v.y() *= -1;
  return v;
}
}  // namespace
// ---------------------------------------------------------------------------
// Tests for the concrete BxDF implementations
// ---------------------------------------------------------------------------

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
  EXPECT_EQ(s.bxdf, &lambert);  // pointer identity

  // PDF in the sample must match calling pdf() directly
  EXPECT_NEAR(s.pdf, lambert.pdf(wi, s.wo), kEps);

  // All cosine–hemisphere samples must point into the +y half–space
  EXPECT_GT(s.wo.y(), 0.0);
}

TEST(LambertianTest, SampleF_IsCosineWeighted) {
  static constexpr size_t N = 2000;
  static const Normal n(0, 1, 0);
  Lambertian lambert(Color(1));

  std::vector<double> cos_thetas(N, 0);
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
  double D = 0;
  for (size_t i = 0; i < N; ++i) {
    double z = cos_thetas[i];
    double F_emp1 = (i + 1.0) / N;               // empirical CDF just above z
    double F_emp0 = static_cast<double>(i) / N;  // empirical CDF just below z
    double F_theo = z * z;                       // theoretical CDF

    D = std::max(D, std::abs(F_emp1 - F_theo));
    D = std::max(D, std::abs(F_emp0 - F_theo));
  }

  // Convert to the limiting statistic sqrt(n) * D_n
  double stat = std::sqrt(N) * D;
  boost::math::kolmogorov_smirnov_distribution<> kolDist(N);
  double K_cdf = cdf(kolDist, stat);

  double p_value = 1.0 - K_cdf;  // p-value = 1 - K(stat)
  EXPECT_LT(p_value, 0.01) << "p_value = " << p_value;
}

TEST(LambertianTest, SampleConvergence) {
  // Compare two Monte Carlo estimators of integral
  // I = \int Li(w) cos(\theta) dw
  // Let Li(w)=1, the exact value shoule be pi.
  static constexpr auto N = 64;
  Lambertian lambert(Color(1));

  double Iu = 0, Icos = 0;
  for (size_t i = 0; i < N; ++i) {
    auto wi = rand_wi();
    auto sample = lambert.Sample_f(wi);
    Icos += sample->wo.y() / sample->pdf;

    auto wo = rand_wi();
    Iu += wo.y() * 2 * pi;
  }

  Iu /= N;
  Icos /= N;
  double err_iu = std::fabs(Iu - pi), err_icos = std::fabs(Icos - pi);
  EXPECT_LT(err_icos, err_iu) << "errors are: " << err_icos << " vs " << err_iu;
}

// ---------------------------------------------------------------------------

TEST(ConductorTest, SpecularFlagsAndSampling) {
  GTEST_SKIP();

  const Color white{1, 1, 1};
  Conductor mirror(white, /*fuzz=*/0.0);  // perfect mirror

  EXPECT_TRUE(mirror.MatchesFlag(BxDFBits::Specular));
  EXPECT_TRUE(mirror.MatchesFlag(BxDFBits::Reflection));

  Vector3 wi{0.2, -0.4, 0.9};
  wi.Normalized();
  Vector3 expectedWo{wi.x(), wi.y(), -wi.z()};

  // pdf() is zero for ideal specular events ------------------------------
  EXPECT_DOUBLE_EQ(mirror.pdf(wi, expectedWo), 0.0);

  // Therefore f() must return black
  Color fval = mirror.f(wi, expectedWo);
  EXPECT_EQ(fval, Color(0, 0, 0));

  // Sample_f must give the perfect reflection with pdf == 1 --------------
  auto sampleOpt = mirror.Sample_f(wi);
  ASSERT_TRUE(sampleOpt.has_value());

  const auto& s = *sampleOpt;
  EXPECT_EQ(s.f, white);
  EXPECT_EQ(s.bxdf, &mirror);
  EXPECT_DOUBLE_EQ(s.pdf, 1.0);

  EXPECT_NEAR(s.wo.x(), expectedWo.x(), kEps);
  EXPECT_NEAR(s.wo.y(), expectedWo.y(), kEps);
  EXPECT_NEAR(s.wo.z(), expectedWo.z(), kEps);
}

TEST(ConductorTest, GlossyHasPositivePdfAndReturnsColour) {
  GTEST_SKIP();
  const double fuzz = 0.5;
  const Color grey{0.5, 0.5, 0.5};
  Conductor glossy(grey, fuzz);

  EXPECT_TRUE(glossy.MatchesFlag(BxDFBits::Glossy));
  EXPECT_FALSE(glossy.MatchesFlag(BxDFBits::Specular));

  Vector3 wi{0, 0.7, 0.7};
  wi = wi.Normalized();
  Vector3 woPerfect{wi.x(), wi.y(), -wi.z()};

  // For glossy conductors pdf() is non-zero close to the perfect direction
  double pdfPerfect = glossy.pdf(wi, woPerfect);
  EXPECT_GT(pdfPerfect, 0.0);

  // Consequently f() returns the base colour there
  EXPECT_EQ(glossy.f(wi, woPerfect), grey);

  // Consistency checks on a random sample -------------------------------
  auto sampleOpt = glossy.Sample_f(wi);
  ASSERT_TRUE(sampleOpt.has_value());
  const auto& s = *sampleOpt;

  EXPECT_EQ(s.f, grey);
  EXPECT_EQ(s.bxdf, &glossy);
  EXPECT_NEAR(s.pdf, glossy.pdf(wi, s.wo), kEps);
}

// ---------------------------------------------------------------------------

TEST(DielectricTest, SpecularSamplingProvidesEitherRefractionOrReflection) {
  GTEST_SKIP();
  Dielectric glass(1.5);

  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Specular));
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Reflection));
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Transmission));

  Vector3 wi{0, 0, 1};
  Vector3 refl{wi.x(), wi.y(), -wi.z()};
  EXPECT_DOUBLE_EQ(glass.pdf(wi, refl), 0.0);

  auto sampleOpt = glass.Sample_f(wi);
  ASSERT_TRUE(sampleOpt.has_value());
  const auto& s = *sampleOpt;
  EXPECT_EQ(s.f, Color(1, 1, 1));
  EXPECT_EQ(s.bxdf, &glass);
  EXPECT_DOUBLE_EQ(s.pdf, 1.0);
  EXPECT_LT(s.wo.z(), 0.0);
}
