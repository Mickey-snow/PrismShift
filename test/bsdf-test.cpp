
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

TEST(ConductorTest, Flags) {
  const Color white{1, 1, 1};
  Conductor mirror(white, /*fuzz=*/0.0);
  EXPECT_TRUE(mirror.MatchesFlag(BxDFBits::Specular));
  EXPECT_TRUE(mirror.MatchesFlag(BxDFBits::Reflection));
}

TEST(ConductorTest, pdfPerfectSpecular) {
  const Color white{1, 1, 1};
  Conductor mirror(white, /*fuzz=*/0.0);

  static constexpr auto N = 16;
  for (size_t i = 0; i < N; ++i) {
    Vector3 wi = rand_sphere_uniform(), wo = rand_sphere_uniform();
    EXPECT_EQ(mirror.f(wi, wo), Color(0));
    EXPECT_NEAR(mirror.pdf(wi, wo), 0, kEps);
  }
}

TEST(ConductorTest, SamplePerfectSpecular) {
  const Color c{0.9, 0.8, 0.7};
  Conductor mirror(c, /*fuzz=*/0.0);

  static constexpr auto N = 16;
  for (size_t i = 0; i < N; ++i) {
    Vector3 wi = rand_sphere_uniform();
    auto sample = mirror.Sample_f(wi);
    ASSERT_TRUE(sample.has_value());
    EXPECT_NEAR(sample->pdf, 1.0, kEps);

    sample->wo.y() = -sample->wo.y();
    EXPECT_EQ(sample->wo, wi);
    EXPECT_EQ(sample->f, c);
  }
}

// ---------------------------------------------------------------------------

TEST(DielectricTest, Flags) {
  Dielectric glass(1.5);
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Specular));
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Reflection));
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Transmission));
}

TEST(DielectricTest, fAndpdf) {
  Dielectric glass(1.6);

  for (int i = 0; i < 10; ++i) {
    Vector3 wi = rand_hemisphere_uniform(), wo = rand_hemisphere_uniform();
    EXPECT_EQ(glass.f(wi, wo), Color(0));
    EXPECT_NEAR(glass.pdf(wi, wo), 0, kEps);
  }
}

TEST(DielectricTest, ReflectAndRefract) {
  static constexpr auto N = 128;
  static const Normal n{0, 1, 0};

  const double eta = 1.6;
  Dielectric glass(eta);

  int reflects = 0, refracts = 0;
  for (int i = 0; i < N; ++i) {
    const Vector3 wi = rand_sphere_uniform();
    auto sample = glass.Sample_f(wi);
    ASSERT_TRUE(sample.has_value());

    const Vector3 wo = sample->wo;
    EXPECT_EQ(Normal(wi.x(), 0, wi.z()), Normal(wo.x(), 0, wo.z()))
        << wi << ' ' << wo;             // phi_r = phi_i + pi
    if (wo.Dot(n) * wi.Dot(n) < 0.0) {  // reflect
      ++reflects;
      EXPECT_NEAR(-wi.y(), wo.y(), kEps)
          << wi << ' ' << wo;  // theta_r = theta_i
    } else {                   // refract
      ++refracts;
      const double e = wi.y() < 0 ? eta : 1.0 / eta;
      EXPECT_NEAR(std::sqrt(1.0 - wi.y() * wi.y()),
                  e * std::sqrt(1.0 - wo.y() * wo.y()), kEps)
          << wi << ' ' << wo;
    }
  }

  EXPECT_GT(reflects, 0);
  EXPECT_GT(refracts, 0);
}
