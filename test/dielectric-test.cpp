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
      : glass(1.5), rough(1.6, TrowbridgeReitzDistribution(0.5, 0.5)) {}

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

TEST_F(DielectricTest, SpecularfAndpdf) {
  for (int i = 0; i < 10; ++i) {
    Vector3 wi = rand_hemisphere_uniform(), wo = rand_hemisphere_uniform();
    EXPECT_EQ(glass.f(wi, wo), Color(0));
    EXPECT_NEAR(glass.pdf(wi, wo), 0, kEps);
  }
}

TEST_F(DielectricTest, RoughfAndpdf) {
  {
    Vector3 wi(0.36001116753526824, 0.9316929280025178, -0.04837610112429691),
        wo(0.07625900273937776, 0.9947810514466787, 0.06778808290426788);
    auto expected_f = 0.625201;
    auto expected_pdf = 0.243103;
    auto result_f = rough.f(wi, wo);
    auto result_pdf = rough.pdf(wi, wo);
    EXPECT_EQ(result_f, Color(expected_f));
    EXPECT_NEAR(result_pdf, expected_pdf, kEps);
  }
  {
    Vector3 wi(-0.10583889543817629, 0.5023951711907252, 0.8581358984314015),
        wo(0.11911528048770384, 0.6781641091855933, 0.725193071510509);
    auto expected_f = 0.600586;
    auto expected_pdf = 0.168523;
    auto result_f = rough.f(wi, wo);
    auto result_pdf = rough.pdf(wi, wo);
    EXPECT_EQ(result_f, Color(expected_f));
    EXPECT_NEAR(result_pdf, expected_pdf, kEps);
  }
  {
    Vector3 wi(0.025676515554949937, 0.997968955904216, -0.058298195515859866),
        wo(-0.5018779354034332, 0.6438252834625484, -0.5775876923286685);
    auto expected_f = 0.00726512;
    auto expected_pdf = 0.00197623;
    auto result_f = rough.f(wi, wo);
    auto result_pdf = rough.pdf(wi, wo);
    EXPECT_EQ(result_f, Color(expected_f));
    EXPECT_NEAR(result_pdf, expected_pdf, kEps);
  }
  {
    Vector3 wi(0.7921375179832313, 0.6100998476707903, 0.017214194010882186),
        wo(0.7625948262817444, 0.3634077880867193, -0.5351484938652394);
    auto expected_f = 0.279227;
    auto expected_pdf = 0.0509515;
    auto result_f = rough.f(wi, wo);
    auto result_pdf = rough.pdf(wi, wo);
    EXPECT_EQ(result_f, Color(expected_f));
    EXPECT_NEAR(result_pdf, expected_pdf, kEps);
  }
  {
    Vector3 wi(0.12744884393243466, 0.9754244770381848, 0.1797328066187963),
        wo(0.48047947925344153, 0.848789253428311, -0.22067232105760806);
    auto expected_f = 0.2565;
    auto expected_pdf = 0.0870532;
    auto result_f = rough.f(wi, wo);
    auto result_pdf = rough.pdf(wi, wo);
    EXPECT_EQ(result_f, Color(expected_f));
    EXPECT_NEAR(result_pdf, expected_pdf, kEps);
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
  // check that
  // \int f(wi,wo)|cos(wo)| / pdf = 1

  double mean = 0;
  int N = 10000, valid = 0;
  for (int i = 0; i < N; ++i) {
    Vector3 wi = rand_sphere_uniform();
    if (auto s = rough.Sample_f(wi)) {
      ++valid;

      double ratio = s->f[0] * AbsCosTheta(s->wo) / s->pdf;  // use red channel
      mean += std::fabs(ratio - 1);
    }
  }
  mean /= valid;

  EXPECT_LT(mean, 0.5);
}

TEST_F(DielectricTest, PdfNormalisation) {
  // check that
  // \int pdf(wi,wo) dwi = 1

  static constexpr int N = 10000;
  double s = 0;
  Vector3 wi = Vector3(0, -1, 0) + 0.1 * rand_sphere_uniform();
  wi = wi.Normalized();
  for (int i = 0; i < N; ++i) {
    Vector3 wo = rand_sphere_uniform();
    double pdf = rough.pdf(wi, wo);
    s += pdf;
  }
  double estimate = (4 * pi) * s / N;
  EXPECT_NEAR(estimate, 1, 0.1);
}

TEST_F(DielectricTest, EnergyConservation) {
  static constexpr int K = 32, M = 10000;

  double reflected = 0, transmitted = 0;
  for (int k = 0; k < K; ++k) {  // incident directions
    Vector3 wi = Vector3(0, -1, 0) + 0.1 * rand_sphere_uniform();
    wi = wi.Normalized();

    for (int j = 0; j < M; ++j) {  // integrate over wo
      if (auto samp = rough.Sample_f(wi)) {
        double cos_o = AbsCosTheta(samp->wo);
        if (SameHemisphere(wi, samp->wo))
          reflected += samp->f[0] * cos_o / samp->pdf;
        else
          transmitted += samp->f[0] * cos_o / samp->pdf;
      }
    }
  }

  int N = K * M;
  reflected /= N;
  transmitted /= N;
  EXPECT_NEAR(reflected + transmitted, 1, 0.1)
      << reflected << ' ' << transmitted;
}
