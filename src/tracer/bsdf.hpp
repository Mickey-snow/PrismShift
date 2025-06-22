#pragma once

#include "util/util.hpp"

#include <memory>
#include <optional>
#include <utility>

enum class [[nodiscard]] BxDFBits : unsigned {
  None = 0,
  Reflection = 1u << 0,
  Transmission = 1u << 1,
  Diffuse = 1u << 2,
  Glossy = 1u << 3,
  Specular = 1u << 4,
  All = Reflection | Transmission | Diffuse | Glossy | Specular
};

constexpr inline BxDFBits operator|(BxDFBits a, BxDFBits b) {
  return static_cast<BxDFBits>(static_cast<unsigned>(a) |
                               static_cast<unsigned>(b));
}
constexpr inline BxDFBits operator&(BxDFBits a, BxDFBits b) {
  return static_cast<BxDFBits>(static_cast<unsigned>(a) &
                               static_cast<unsigned>(b));
}
inline BxDFBits& operator|=(BxDFBits& a, BxDFBits b) {
  a = a | b;
  return a;
}
inline BxDFBits& operator&=(BxDFBits& a, BxDFBits b) {
  a = a & b;
  return a;
}

class BxDF;
struct bxdfSample {
  Color f;
  Vector3 wo;
  Float pdf;
  BxDFBits flag;
};

class BxDF {
 public:
  virtual ~BxDF() = default;
  explicit BxDF(BxDFBits _flag) : flag(_flag) {}

  // returns the value of the distribution function for the given pair of
  // directions
  virtual Color f(const Vector3& wi, const Vector3& wo) const = 0;

  // a method that uses importance sampling to draw a direction from a
  // distribution that approximately matches the scattering function's shape
  virtual std::optional<bxdfSample> Sample_f(const Vector3& wi) const = 0;

  // returns the value of the probability density function for the given pair of
  // directions
  virtual Float pdf(const Vector3& wi, const Vector3& wo) const = 0;

  bool MatchesFlag(BxDFBits flag) const {
    return (this->flag & flag) != BxDFBits::None;
  }
  BxDFBits GetFlags(void) const { return this->flag; }

 private:
  BxDFBits flag{BxDFBits::None};

 protected:
  template <typename T>
  BxDF& SetFlags(T&& _flag) {
    flag = static_cast<BxDFBits>(std::forward<T>(_flag));
    return *this;
  }
};

struct HitRecord;
class IMaterial;
class BSDF {
 public:
  explicit BSDF(std::shared_ptr<BxDF> bxdf, QuaternionTransform trans);

  Color f(Vector3 wi, Vector3 wo, BxDFBits flag = BxDFBits::All) const;
  std::optional<bxdfSample> Sample_f(Vector3 wi) const;
  Float pdf(Vector3 wi, Vector3 wo, BxDFBits flag = BxDFBits::All) const;
  bool MatchesFlag(BxDFBits flag) const;

 private:
  std::shared_ptr<BxDF> bxdf_;
  QuaternionTransform transform;
};
