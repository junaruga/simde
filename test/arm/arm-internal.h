#if !defined(SIMDE__ARM_INTERNAL_H)
#define SIMDE__ARM_INTERNAL_H

#include "../../simde/hedley.h"

#if HEDLEY_HAS_WARNING("-Wdouble-promotion")
#  pragma clang diagnostic ignored "-Wpadded"
#endif

#include "../test.h"
#include "../../simde/arm/neon.h"

#include <math.h>

#if !defined(SIMDE_NO_NATIVE)
#  define NEON_TEST_SUITE(name) simde_neon_##name##_emul_test_suite
#else
#  define NEON_TEST_SUITE(name) simde_neon_##name##_test_suite
#endif

#define SIMDE_NEON_DECLARE_TEST_SUITE(name) \
  HEDLEY_C_DECL MunitSuite simde_neon_##name##_test_suite; \
  HEDLEY_C_DECL MunitSuite simde_neon_##name##_emul_test_suite;

SIMDE_NEON_DECLARE_TEST_SUITE(vadd)
SIMDE_NEON_DECLARE_TEST_SUITE(vdup_n)
SIMDE_NEON_DECLARE_TEST_SUITE(vmul)
SIMDE_NEON_DECLARE_TEST_SUITE(vsub)

#define simde_neon_assert_int8x8(a, op, b) \
  simde_assert_typev(int8_t, PRId8, (sizeof(a) / sizeof(int8_t)), (const int8_t*) &(a), op, (const int8_t*) &(b))
#define simde_neon_assert_int16x4(a, op, b) \
  simde_assert_typev(int16_t, PRId16, (sizeof(a) / sizeof(int16_t)), (const int16_t*) &(a), op, (const int16_t*) &(b))
#define simde_neon_assert_int32x2(a, op, b) \
  simde_assert_typev(int32_t, PRId32, (sizeof(a) / sizeof(int32_t)), (const int32_t*) &(a), op, (const int32_t*) &(b))
#define simde_neon_assert_int64x1(a, op, b) \
  simde_assert_typev(int64_t, PRId64, (sizeof(a) / sizeof(int64_t)), (const int64_t*) &(a), op, (const int64_t*) &(b))
#define simde_neon_assert_uint8x8(a, op, b) \
  simde_assert_typev(uint8_t, PRId8, (sizeof(a) / sizeof(uint8_t)), (const uint8_t*) &(a), op, (const uint8_t*) &(b))
#define simde_neon_assert_uint16x4(a, op, b) \
  simde_assert_typev(uint16_t, PRId16, (sizeof(a) / sizeof(uint16_t)), (const uint16_t*) &(a), op, (const uint16_t*) &(b))
#define simde_neon_assert_uint32x2(a, op, b) \
  simde_assert_typev(uint32_t, PRId32, (sizeof(a) / sizeof(uint32_t)), (const uint32_t*) &(a), op, (const uint32_t*) &(b))
#define simde_neon_assert_uint64x1(a, op, b) \
  simde_assert_typev(uint64_t, PRId64, (sizeof(a) / sizeof(uint64_t)), (const uint64_t*) &(a), op, (const uint64_t*) &(b))
#define simde_neon_assert_float32x2(a, op, b) \
  simde_assert_typev(simde_float32, "f", (sizeof(a) / sizeof(simde_float32)), (const simde_float32*) &(a), op, (const simde_float32*) &(b))
#define simde_neon_assert_float32x2_equal(a, b, precision) \
  simde_assert_f32v_equal(simde_float32, (sizeof(a) / sizeof(simde_float32)), (const simde_float32*) &(a), (const simde_float32*) &(b), precision)
#define simde_neon_assert_float64x1(a, op, b) \
  simde_assert_typev(simde_float64, "f", (sizeof(a) / sizeof(simde_float64)), (const simde_float64*) &(a), op, (const simde_float64*) &(b))
#define simde_neon_assert_float64x1_equal(a, b, precision) \
  simde_assert_f32v_equal(simde_float64, (sizeof(a) / sizeof(simde_float64)), (const simde_float64*) &(a), (const simde_float64*) &(b), precision)

#define simde_neon_assert_int8x16(a, op, b) \
  simde_assert_typev(int8_t, PRId8, (sizeof(a) / sizeof(int8_t)), (const int8_t*) &(a), op, (const int8_t*) &(b))
#define simde_neon_assert_int16x8(a, op, b) \
  simde_assert_typev(int16_t, PRId16, (sizeof(a) / sizeof(int16_t)), (const int16_t*) &(a), op, (const int16_t*) &(b))
#define simde_neon_assert_int32x4(a, op, b) \
  simde_assert_typev(int32_t, PRId32, (sizeof(a) / sizeof(int32_t)), (const int32_t*) &(a), op, (const int32_t*) &(b))
#define simde_neon_assert_int64x2(a, op, b) \
  simde_assert_typev(int64_t, PRId64, (sizeof(a) / sizeof(int64_t)), (const int64_t*) &(a), op, (const int64_t*) &(b))
#define simde_neon_assert_uint8x16(a, op, b) \
  simde_assert_typev(uint8_t, PRId8, (sizeof(a) / sizeof(uint8_t)), (const uint8_t*) &(a), op, (const uint8_t*) &(b))
#define simde_neon_assert_uint16x8(a, op, b) \
  simde_assert_typev(uint16_t, PRId16, (sizeof(a) / sizeof(uint16_t)), (const uint16_t*) &(a), op, (const uint16_t*) &(b))
#define simde_neon_assert_uint32x4(a, op, b) \
  simde_assert_typev(uint32_t, PRId32, (sizeof(a) / sizeof(uint32_t)), (const uint32_t*) &(a), op, (const uint32_t*) &(b))
#define simde_neon_assert_uint64x2(a, op, b) \
  simde_assert_typev(uint64_t, PRId64, (sizeof(a) / sizeof(uint64_t)), (const uint64_t*) &(a), op, (const uint64_t*) &(b))
#define simde_neon_assert_float32x4(a, op, b) \
  simde_assert_typev(simde_float32, "f", (sizeof(a) / sizeof(simde_float32)), (const simde_float32*) &(a), op, (const simde_float32*) &(b))
#define simde_neon_assert_float32x4_equal(a, b, precision) \
  simde_assert_f32v_equal(simde_float32, (sizeof(a) / sizeof(simde_float32)), (const simde_float32*) &(a), (const simde_float32*) &(b), precision)
#define simde_neon_assert_float64x2(a, op, b) \
  simde_assert_typev(simde_float64, "f", (sizeof(a) / sizeof(simde_float64)), (const simde_float64*) &(a), op, (const simde_float64*) &(b))
#define simde_neon_assert_float64x2_equal(a, b, precision) \
  simde_assert_f32v_equal(simde_float64, (sizeof(a) / sizeof(simde_float64)), (const simde_float64*) &(a), (const simde_float64*) &(b), precision)

#define SIMDE_NEON_GEN_RAND_ARRAY_FUNC(L, N, V)	\
  SIMDE__FUNCTION_ATTRIBUTES \
  simde_float##L##x##N##_t \
  simde_neon_random_float##L##x##N(void) { \
    simde_float##L v[sizeof(simde_float##L##x##N##_t) / sizeof(simde_float##L)]; \
    for (size_t i = 0 ; i < sizeof(v) / sizeof(v[0]) ; i++) { \
      v[i] = (simde_float##L) (round(random_f64_range(-1000.0, 1000.0) * 100.0) / 100.0); \
    } \
    return simde_vld1##V##_f##L(v); \
  }

SIMDE__FUNCTION_ATTRIBUTES
simde_float32x2_t
simde_neon_random_float32x2(void) {
  simde_float32 v[sizeof(simde_float32x2_t) / sizeof(simde_float32)];
  for (size_t i = 0 ; i < sizeof(v) / sizeof(v[0]) ; i++) {
    v[i] = (simde_float32) (round(random_f64_range(-1000.0, 1000.0) * 100.0) / 100.0);
  }
  return simde_vld1_f32(v);
}

SIMDE__FUNCTION_ATTRIBUTES
simde_float64x1_t
simde_neon_random_float64x1(void) {
  simde_float64 v[sizeof(simde_float64x1_t) / sizeof(simde_float64)];
  for (size_t i = 0 ; i < sizeof(v) / sizeof(v[0]) ; i++) {
    v[i] = (simde_float64) (round(random_f64_range(-1000.0, 1000.0) * 100.0) / 100.0);
  }
  return simde_vld1_f64(v);
}

SIMDE__FUNCTION_ATTRIBUTES
simde_float32x4_t
simde_neon_random_float32x4(void) {
  simde_float32 v[sizeof(simde_float32x4_t) / sizeof(simde_float32)];
  for (size_t i = 0 ; i < sizeof(v) / sizeof(v[0]) ; i++) {
    v[i] = (simde_float32) (round(random_f64_range(-1000.0, 1000.0) * 100.0) / 100.0);
  }
  return simde_vld1q_f32(v);
}

SIMDE__FUNCTION_ATTRIBUTES
simde_float64x2_t
simde_neon_random_float64x2(void) {
  simde_float64 v[sizeof(simde_float64x2_t) / sizeof(simde_float64)];
  for (size_t i = 0 ; i < sizeof(v) / sizeof(v[0]) ; i++) {
    v[i] = (simde_float64) (round(random_f64_range(-1000.0, 1000.0) * 100.0) / 100.0);
  }
  return simde_vld1q_f64(v);
}

#endif
