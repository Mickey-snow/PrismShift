#pragma once

#include <memory>
#include <optional>
#include <util/util.hpp>
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
  double pdf;
  BxDF const* bxdf;

  bxdfSample() : f{0, 0, 0}, wo{0, 0, 0}, pdf(0.0), bxdf(nullptr) {}
  bxdfSample(const Color& _col,
             const Vector3& _rout,
             const double& _pdf,
             BxDF const* _bxdf)
      : f(_col), wo(_rout), pdf(_pdf), bxdf(_bxdf) {}
};

class BxDF {
 public:
  virtual ~BxDF() = default;
  explicit BxDF(BxDFBits _flag) : flag(_flag) {}

  // returns the value of the distribution function for the given pair of
  // directions
  virtual Color f(const Vector3&, const Vector3&) const = 0;

  // a method that uses importance sampling to draw a direction from a
  // distribution that approximately matches the scattering function's shape
  virtual std::optional<bxdfSample> Sample_f(const Vector3& rin) const = 0;

  // returns the value of the probability density function for the given pair of
  // directions
  virtual double pdf(const Vector3& in_direction,
                     const Vector3& out_direction) const = 0;

  bool MatchesFlag(BxDFBits flag) const {
    return (this->flag & flag) != BxDFBits::None;
  }
  BxDFBits GetFlags(void) const { return this->flag; }

 private:
  BxDFBits flag{BxDFBits::None};

 protected:
  template <typename... Ts>
  bxdfSample Make_Sample(Ts&&... params) const {
    return bxdfSample(std::forward<Ts>(params)...);
  }

  template <typename T>
  BxDF& SetFlags(T&& _flag) {
    flag = static_cast<BxDFBits>(std::forward<T>(_flag));
    return *this;
  }
};

class BSDF {
 public:
  explicit BSDF() { bxdf = nullptr; }
  explicit BSDF(std::shared_ptr<BxDF> _bxdf) : bxdf(std::move(_bxdf)) {}

  virtual Color f(const Vector3&,
                  const Vector3&,
                  BxDFBits flag = BxDFBits::All) const;
  virtual std::optional<bxdfSample> Sample_f(const Vector3&) const;
  virtual double pdf(const Vector3&,
                     const Vector3&,
                     BxDFBits flag = BxDFBits::All) const;

 private:
  std::shared_ptr<BxDF> bxdf;
};
