#include <gtest/gtest.h>

#include "trowbridge_reitz_distribution.hpp"
#include "util/util.hpp"

#include <cmath>
#include <utility>

static constexpr double EPS = 1e-6;
static constexpr double kEpsilon = 1e-6;

TEST(TrowbridgeReitz, D_HorizonReturnsZero) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);
  Vector3 m(0, 0, 1);  // cos(theta)=0 => tan^2(theta)=inf
  EXPECT_DOUBLE_EQ(dist.D(m), 0.0);
}

TEST(TrowbridgeReitz, D_NormalIncidence) {
  const double ax = 0.3, ay = 0.4;
  TrowbridgeReitzDistribution dist(ax, ay);
  Vector3 m(0, 1, 0);
  // At normal incidence: tan^2(theta)=0, cos^4(theta)=1, e=0 => D =
  // 1/(pi*ax*ay)
  const double expected = 1.0 / (pi * ax * ay);
  EXPECT_NEAR(dist.D(m), expected, EPS);
}

TEST(TrowbridgeReitz, LambdaG1_NormalDirection) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);
  Vector3 w(0, 1, 0);
  EXPECT_DOUBLE_EQ(dist.Lambda(w), 0.0);
  EXPECT_DOUBLE_EQ(dist.G1(w), 1.0);
}

TEST(TrowbridgeReitz, LambdaG1_HorizonDirection) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);
  Vector3 w(1, 0, 0);
  // tan^2(theta) infinite => Lambda=0, G1=1
  EXPECT_DOUBLE_EQ(dist.Lambda(w), 0.0);
  EXPECT_DOUBLE_EQ(dist.G1(w), 1.0);
}

TEST(TrowbridgeReitz, G_CombinedGeometry) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);
  Vector3 wi(0, -1, 0), wo(0, 1, 0);
  EXPECT_DOUBLE_EQ(dist.G(wi, wo), 1.0);
}

TEST(TrowbridgeReitz, PDFEqualsD) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);
  Vector3 w(0, 1, 0), wm(0, 1, 0);
  EXPECT_DOUBLE_EQ(dist.PDF(w, wm), dist.D(w, wm));
}

TEST(TrowbridgeReitz, EffectivelySmooth) {
  TrowbridgeReitzDistribution smoothDist(1e-4, 1e-4);
  EXPECT_TRUE(smoothDist.EffectivelySmooth());
  TrowbridgeReitzDistribution roughDist(1e-2, 1e-4);
  EXPECT_FALSE(roughDist.EffectivelySmooth());
}

TEST(TrowbridgeReitz, RoughnessToAlpha) {
  double r = 0.36;
  EXPECT_DOUBLE_EQ(TrowbridgeReitzDistribution::RoughnessToAlpha(r), 0.6);
}

TEST(TrowbridgeReitzDistribution, VisibleNormalPDF) {
  constexpr int N = 200'000;  // Monte-Carlo budget
  constexpr std::tuple<double, double> roughness[] = {
      {0.1, 0.1}, {0.25, 0.45}, {0.5, 1.0}, {1.0, 1.0}};

  // Use a *fixed* outgoing direction so that the estimator variance is
  // dominated by the micro-facet sampling, not the choice of wo.
  const Vector3 wo(0.0, 1.0, 0.0);

  for (auto [ax, az] : roughness) {
    TrowbridgeReitzDistribution dist(ax, az);

    double sum = 0.0;
    double sum2 = 0.0;

    for (int i = 0; i < N; ++i) {
      const Vector3 wm = dist.Sample_wm(wo);
      const double pdf = dist.PDF(wo, wm);

      ASSERT_GT(pdf, 0.0) << "PDF should never be zero or negative.";

      const double inv = 1.0 / pdf;
      sum += inv;
      sum2 += inv * inv;
    }

    const double mean = sum / N;
    const double var = sum2 / N - mean * mean;
    const double se = std::sqrt(var / N);  // standard error

    EXPECT_NEAR(mean, 2 * pi, 3.0 * se)
        << "ax=" << ax << "  az=" << az << "  mean=" << mean
        << "  3se=" << 3.0 * se;
  }
}
