#include <gtest/gtest.h>

#include "trowbridge_reitz_distribution.hpp"
#include "util/util.hpp"

#include <cmath>
#include <utility>

static constexpr Float EPS = 1e-5;

TEST(TrowbridgeReitz, D) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);

  Vector3 v(-0.137809, -0.990454, 0.00295599);
  auto result = dist.D(v);
  EXPECT_NEAR(result, 1.13962, EPS);
}

TEST(TrowbridgeReitz, LambdaG1) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);

  Vector3 v(-0.0767713, 0.465353, 0.881789);
  auto result = dist.Lambda(v);
  EXPECT_NEAR(result, 0.190009, EPS);

  v = Vector3(-0.0362931, 0.98403, 0.174266);
  result = dist.Lambda(v);
  EXPECT_NEAR(result, 0.00204098, EPS);

  v = Vector3(-0.749965, -0.633004, -0.191985);
  result = dist.Lambda(v);
  EXPECT_NEAR(result, 0.0860713, EPS);
}

TEST(TrowbridgeReitz, G) {
  TrowbridgeReitzDistribution dist(0.5, 0.5);

  {
    Vector3 wi(0.34772845107931405, 0.6978936396675169, -0.6261225056022242),
        wo(0.9120582051189103, 0.2038389956207956, -0.3558082269138487);
    auto result = dist.G(wi, wo);
    EXPECT_NEAR(result, 0.5368741, EPS);
  }
  {
    Vector3 wi(0.4775231358733295, 0.20832347561723202, 0.8535648681924873),
        wo(0.45715199890750996, 0.06493455289117583, 0.8870149681576355);
    auto result = dist.G(wi, wo);
    EXPECT_NEAR(result, 0.19417125, EPS);
  }
  {
    Vector3 wi(-0.6630024855049268, 0.565152141359709, 0.49094883779353726),
        wo(-0.5962846529988767, 0.7081244934667106, -0.3781591124784962);
    auto result = dist.G(wi, wo);
    EXPECT_NEAR(result, 0.84910387, EPS);
  }
  {
    Vector3 wi(0.2052984677929336, -0.8523450355584156, 0.4809994589194433),
        wo(0.5038692457176487, 0.40180420369561975, -0.764636622921278);
    auto result = dist.G(wi, wo);
    EXPECT_NEAR(result, 0.78061515, EPS);
  }
  {
    Vector3 wi(0.8765415296916448, 0.3977303034200664, -0.27108218766125725),
        wo(0.8904233370506345, -0.398334444091023, -0.22017254934779618);
    auto result = dist.G(wi, wo);
    EXPECT_NEAR(result, 0.65540475, EPS);
  }
}

TEST(TrowbridgeReitz, PDFEqualsD) {
  TrowbridgeReitzDistribution dist(0.5, 0.6);
  for (int i = 0; i < 10; ++i) {
    Vector3 w = rand_sphere_uniform(), wm = rand_sphere_uniform();
    EXPECT_DOUBLE_EQ(dist.PDF(w, wm), dist.D(w, wm));
  }
}

TEST(TrowbridgeReitz, EffectivelySmooth) {
  TrowbridgeReitzDistribution smoothDist(1e-4, 1e-4);
  EXPECT_TRUE(smoothDist.EffectivelySmooth());
  TrowbridgeReitzDistribution roughDist(1e-2, 1e-4);
  EXPECT_FALSE(roughDist.EffectivelySmooth());
}

TEST(TrowbridgeReitz, RoughnessToAlpha) {
  Float r = 0.36;
  EXPECT_DOUBLE_EQ(TrowbridgeReitzDistribution::RoughnessToAlpha(r), 0.6);
}

TEST(TrowbridgeReitzDistribution, VisibleNormalPDF) {
  constexpr int N = 200'000;  // Monte-Carlo budget
  constexpr std::tuple<Float, Float> roughness[] = {
      {0.1, 0.1}, {0.25, 0.45}, {0.5, 1.0}, {1.0, 1.0}};

  for (auto [ax, az] : roughness) {
    // Use a *fixed* outgoing direction so that the estimator variance is
    // dominated by the micro-facet sampling, not the choice of wo.
    const Vector3 up(0.0, 1.0, 0.0);

    TrowbridgeReitzDistribution dist(ax, az);

    Float sum = 0.0;
    Float sum2 = 0.0;

    for (int i = 0; i < N; ++i) {
      const Vector3 wm = dist.Sample_wm(up);
      const Float pdf = dist.PDF(up, wm);

      ASSERT_GT(pdf, 0.0) << "PDF should never be zero or negative.";

      const Float inv = 1.0 / pdf;
      sum += inv;
      sum2 += inv * inv;
    }

    const Float mean = sum / N;
    const Float var = sum2 / N - mean * mean;
    const Float se = std::sqrt(var / N);  // standard error

    EXPECT_NEAR(mean, 2 * pi, 6 * se)
        << "ax=" << ax << "  az=" << az << "  mean=" << mean
        << "  3se=" << 3.0 * se;
  }
}
