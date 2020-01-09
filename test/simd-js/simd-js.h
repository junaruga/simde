#if !defined(SIMDE__JS_INTERNAL_H)
#define SIMDE__JS_INTERNAL_H

#include "../../simde/hedley.h"

#if HEDLEY_HAS_WARNING("-Wpadded")
#  pragma clang diagnostic ignored "-Wpadded"
#endif
#if HEDLEY_HAS_WARNING("-Wmissing-variable-declarations")
#  pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif
#if HEDLEY_HAS_WARNING("-Wextra-semi")
#  pragma clang diagnostic ignored "-Wextra-semi"
#endif

#include "../test.h"

#if defined(SIMDE_NO_NATIVE)
#  define SIMD_JS_TEST_FUNC(type, name) \
  { (char*) "/" #name "/emul", test_simde_em_##type##_##name, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
#else
#  define SIMD_JS_TEST_FUNC(type, name) \
  { (char*) "/" #name, test_simde_em_##type##_##name, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
#endif
#define SIMD_JS_TEST_FUNC_CMP(type, name) \
  { (char*) "/" #name "/cmp", test_simde_em_##type##_##name, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

MunitSuite simde_simd_js_int32x4_test_suite;
MunitSuite simde_simd_js_int32x4_emul_test_suite;
MunitSuite simde_simd_js_int32x4_cmp_test_suite;

MunitSuite simde_simd_js_float32x4_test_suite;

#endif
