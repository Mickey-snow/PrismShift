#pragma once

#ifdef ENABLE_SIMD
#include <immintrin.h>
#include <cstddef>

namespace simd {

// load/store helpers
inline void store(double* dst, __m128d v) { _mm_storeu_pd(dst, v); }
inline void store(double* dst, __m256d v) { _mm256_storeu_pd(dst, v); }

inline __m128d load2(const double* p) { return _mm_loadu_pd(p); }
inline __m256d load4(const double* p) { return _mm256_loadu_pd(p); }

// --- add -------------------------------------------------------------------
template <std::size_t N>
inline void add(const double* a, const double* b, double* dst);

template <>
inline void add<2>(const double* a, const double* b, double* dst) {
  store(dst, _mm_add_pd(load2(a), load2(b)));
}

template <>
inline void add<3>(const double* a, const double* b, double* dst) {
  store(dst, _mm_add_pd(load2(a), load2(b)));
  dst[2] = a[2] + b[2];
}

template <>
inline void add<4>(const double* a, const double* b, double* dst) {
  store(dst, _mm256_add_pd(load4(a), load4(b)));
}

// --- multiply (element-wise) ----------------------------------------------
template <std::size_t N>
inline void mul(const double* a, const double* b, double* dst);

template <>
inline void mul<2>(const double* a, const double* b, double* dst) {
  store(dst, _mm_mul_pd(load2(a), load2(b)));
}

template <>
inline void mul<3>(const double* a, const double* b, double* dst) {
  store(dst, _mm_mul_pd(load2(a), load2(b)));
  dst[2] = a[2] * b[2];
}

template <>
inline void mul<4>(const double* a, const double* b, double* dst) {
  store(dst, _mm256_mul_pd(load4(a), load4(b)));
}

// --- dot -------------------------------------------------------------------
template <std::size_t N>
inline double dot(const double* a, const double* b);

template <>
inline double dot<2>(const double* a, const double* b) {
  __m128d mul = _mm_mul_pd(load2(a), load2(b));
  __m128d sum = _mm_hadd_pd(mul, mul);
  return _mm_cvtsd_f64(sum);
}

template <>
inline double dot<3>(const double* a, const double* b) {
  __m128d mul = _mm_mul_pd(load2(a), load2(b));
  __m128d sum = _mm_hadd_pd(mul, mul);
  double res = _mm_cvtsd_f64(sum);
  return res + a[2] * b[2];
}

template <>
inline double dot<4>(const double* a, const double* b) {
  __m256d mul = _mm256_mul_pd(load4(a), load4(b));
  __m128d hi = _mm256_extractf128_pd(mul, 1);
  __m128d lo = _mm256_castpd256_pd128(mul);
  __m128d sum2 = _mm_add_pd(hi, lo);
  __m128d sum = _mm_hadd_pd(sum2, sum2);
  return _mm_cvtsd_f64(sum);
}

}  // namespace simd

#endif  // ENABLE_SIMD
