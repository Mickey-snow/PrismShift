#include <gtest/gtest.h>

#include "bsdf.hpp"
#include "bxdfs/conductor.hpp"
#include "util/util.hpp"

using namespace vec_helpers;
using namespace bxdfs;

static constexpr auto kEps = 1e-6;

TEST(ConductorTest, Flags) {
  const Color white{1, 1, 1};
  Conductor mirror(TrowbridgeReitzDistribution(0, 0), white);
  EXPECT_TRUE(mirror.MatchesFlag(BxDFBits::Specular));
  EXPECT_TRUE(mirror.MatchesFlag(BxDFBits::Reflection));
}

TEST(ConductorTest, pdfPerfectSpecular) {
  const Color white{1, 1, 1};
  Conductor mirror(TrowbridgeReitzDistribution(0, 0), white);

  static constexpr auto N = 16;
  for (size_t i = 0; i < N; ++i) {
    Vector3 wi = rand_sphere_uniform(), wo = rand_sphere_uniform();
    EXPECT_EQ(mirror.f(wi, wo), Color(0));
    EXPECT_NEAR(mirror.pdf(wi, wo), 0, kEps);
  }
}

TEST(ConductorTest, SamplePerfectSpecular) {
  const Color c{0.9, 0.8, 0.7};
  Conductor mirror(TrowbridgeReitzDistribution(0, 0), c);

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
