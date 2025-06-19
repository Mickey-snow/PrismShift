#include <gtest/gtest.h>

#include "bsdf.hpp"
#include "bxdfs/dielectric.hpp"
#include "util/util.hpp"

using namespace vec_helpers;
using namespace bxdfs;

static constexpr auto kEps = 1e-6;

class DielectricTest : public ::testing::Test {
 protected:
  DielectricTest()
      : glass(1.5), rough(1.6, TrowbridgeReitzDistribution(0.01, 0.01)) {}

  Dielectric glass;
  Dielectric rough;

  inline static constexpr auto N = 128;
  inline static const Normal n{0, 1, 0};
};

TEST_F(DielectricTest, Flags) {
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Specular));
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Reflection));
  EXPECT_TRUE(glass.MatchesFlag(BxDFBits::Transmission));
}

TEST_F(DielectricTest, fAndpdf) {
  for (int i = 0; i < 10; ++i) {
    Vector3 wi = rand_hemisphere_uniform(), wo = rand_hemisphere_uniform();
    EXPECT_EQ(glass.f(wi, wo), Color(0));
    EXPECT_NEAR(glass.pdf(wi, wo), 0, kEps);
  }
}

TEST_F(DielectricTest, ReflectAndRefract) {
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

TEST_F(DielectricTest, PdfBsdfConsistency) {
  // FIXME
  GTEST_SKIP();

  // check that
  // \int f(wi,wo)|cos(wo)| / pdf = 1
  double worst = 1, mean = 0;
  int N = 1000000, valid = 0;
  for (int i = 0; i < N; ++i) {
    Vector3 wi = rand_sphere_uniform();
    if (auto s = rough.Sample_f(wi)) {
      ++valid;
      double ratio = s->f[0] * AbsCosTheta(s->wo) / s->pdf;  // use red channel
      if (std::fabs(ratio - 1) > std::fabs(worst - 1))
        worst = ratio;
      mean += std::fabs(ratio - 1);
    }
  }
  mean /= valid;

  EXPECT_NEAR(worst, 1, 1e-2);
  EXPECT_LT(mean, 1e-2);
}

TEST_F(DielectricTest, PdfNormalisation) {
  // FIXME
  GTEST_SKIP();

  // check that
  // \int pdf(wi,wo) dwi = 1

  double s = 0;
  Vector3 wi = rand_sphere_uniform();
  static constexpr int N = 100000;
  for (int j = 0; j < N; ++j) {
    Vector3 wo = rand_sphere_uniform();
    s += rough.pdf(wi, wo);
  }
  double estimate = (4 * pi) * s / N;
  EXPECT_NEAR(estimate, 1, 0.1);
}

TEST_F(DielectricTest, EnergyConservation) {
  // FIXME
  GTEST_SKIP();

  static constexpr int K = 32, M = 10000;
  int valid_cnt = 0;

  double reflected = 0, transmitted = 0;
  for (int k = 0; k < K; ++k) {  // incident directions
    Vector3 wi = rand_sphere_uniform();
    for (int j = 0; j < M; ++j) {  // integrate over wo
      if (auto samp = rough.Sample_f(wi)) {
        ++valid_cnt;
        double cos_o = AbsCosTheta(samp->wo);
        if (SameHemisphere(wi, samp->wo))
          reflected += samp->f[0] * cos_o / samp->pdf;
        else
          transmitted += samp->f[0] * cos_o / samp->pdf;
      }
    }
  }

  reflected /= valid_cnt;
  transmitted /= valid_cnt;
  EXPECT_NEAR(reflected + transmitted, 1, 0.01)
      << reflected << ' ' << transmitted;
}
