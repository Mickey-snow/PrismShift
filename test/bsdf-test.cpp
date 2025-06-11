
#include <gtest/gtest.h>

#include "bsdf.hpp"
#include "bxdfs/conductor.hpp"
#include "bxdfs/dielectric.hpp"
#include "bxdfs/lambertian.hpp"
#include "util/util.hpp"

using namespace bxdfs;

constexpr double kEps = 1e-6;

static void ExpectColorEq(const Color& a, const Color& b, double eps = kEps) {
  EXPECT_NEAR(a.r(), b.r(), eps);
  EXPECT_NEAR(a.g(), b.g(), eps);
  EXPECT_NEAR(a.b(), b.b(), eps);
}

// ---------------------------------------------------------------------------
// Tests for the concrete BxDF implementations
// ---------------------------------------------------------------------------

TEST(Lambertian, F_ReturnsConstantColour) {
  const Color red{1.0, 0.0, 0.0};
  Lambertian lambert(red);

  Color val = lambert.f(/*wi*/ {0, 0, 1}, /*wo*/ {1, 0, 1});
  ExpectColorEq(val, red * invpi);
  val = lambert.f({0, 0, -1}, {1, 0, 1});
  ExpectColorEq(val, Color());
}

TEST(Lambertian, SampleF_ProvidesSelfConsistentSample) {
  const Color c{0.2, 0.6, 0.8};
  Lambertian lambert(c);

  Vector3 wi{0, 0, 1};  // normal incidence
  auto sampleOpt = lambert.Sample_f(wi);
  ASSERT_TRUE(sampleOpt.has_value());

  const auto& s = *sampleOpt;
  ExpectColorEq(s.f, c * invpi);
  EXPECT_EQ(s.bxdf, &lambert);  // pointer identity

  // PDF in the sample must match calling pdf() directly
  EXPECT_NEAR(s.pdf, lambert.pdf(wi, s.wo), kEps);

  // All cosine–hemisphere samples must point into the +Z half–space
  EXPECT_GT(s.wo.z(), 0.0);
}

TEST(Conductor, SpecularFlagsAndSampling) {
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
  ExpectColorEq(fval, {0, 0, 0});

  // Sample_f must give the perfect reflection with pdf == 1 --------------
  auto sampleOpt = mirror.Sample_f(wi);
  ASSERT_TRUE(sampleOpt.has_value());

  const auto& s = *sampleOpt;
  ExpectColorEq(s.f, white);
  EXPECT_EQ(s.bxdf, &mirror);
  EXPECT_DOUBLE_EQ(s.pdf, 1.0);

  EXPECT_NEAR(s.wo.x(), expectedWo.x(), kEps);
  EXPECT_NEAR(s.wo.y(), expectedWo.y(), kEps);
  EXPECT_NEAR(s.wo.z(), expectedWo.z(), kEps);
}

TEST(Conductor, GlossyHasPositivePdfAndReturnsColour) {
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
  ExpectColorEq(glossy.f(wi, woPerfect), grey);

  // Consistency checks on a random sample -------------------------------
  auto sampleOpt = glossy.Sample_f(wi);
  ASSERT_TRUE(sampleOpt.has_value());
  const auto& s = *sampleOpt;

  ExpectColorEq(s.f, grey);
  EXPECT_EQ(s.bxdf, &glossy);
  EXPECT_NEAR(s.pdf, glossy.pdf(wi, s.wo), kEps);
}

TEST(Dielectric, SpecularSamplingProvidesEitherRefractionOrReflection) {
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
  ExpectColorEq(s.f, {1, 1, 1});
  EXPECT_EQ(s.bxdf, &glass);
  EXPECT_DOUBLE_EQ(s.pdf, 1.0);
  EXPECT_LT(s.wo.z(), 0.0);
}

// ---------------------------------------------------------------------------
// Tests for the BSDF wrapper/aggregator
// ---------------------------------------------------------------------------

TEST(BSDF, EmptyBsdfReturnsBlackAndZero) {
  BSDF bsdf;  // no bxdf attached

  Vector3 wi{0, 0, 1}, wo{0, 1, 0};

  ExpectColorEq(bsdf.f(wi, wo), {0, 0, 0});
  EXPECT_DOUBLE_EQ(bsdf.pdf(wi, wo), 0.0);
  EXPECT_FALSE(bsdf.Sample_f(wi).has_value());
}
