#if !defined(SIMDE_TESTS_JS_INTERNAL_H)
#define SIMDE_TESTS_CURRENT_ARCH em

#include "../run-tests.h"

HEDLEY_BEGIN_C_DECLS

#define SIMDE_TESTS_GENERATE_SUITE_GETTERS(isax) \
  MunitSuite* SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, native, c)(void); \
  MunitSuite* SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, native, cpp)(void); \
  MunitSuite* SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, emul,   c)(void); \
  MunitSuite* SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, emul,   cpp)(void)

SIMDE_TESTS_GENERATE_SUITE_GETTERS(int32x4);
SIMDE_TESTS_GENERATE_SUITE_GETTERS(float32x4);

#define SIMDE_JS_TESTS_DEFINE_TEST(name) \
  { (char*) SIMDE_TESTS_GENERATE_NAME(name), SIMDE_TESTS_CONCAT3(SIMDE_TESTS_CONCAT3(test_simde, _, SIMDE_TESTS_CURRENT_ARCH), _, SIMDE_TESTS_CONCAT3(SIMDE_TESTS_CURRENT_ISAX, _, name)), NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

#define SIMDE_JS_DEFINE_TEST_EM_BINARY(T, op)				\
  static MunitResult                                                    \
  test_simde_em_##T##_##op(const MunitParameter params[], void* data) { \
    (void) params;                                                      \
    (void) data;                                                        \
                                                                        \
    T na, nb, nr;                                                       \
    simde_em_##T ea, eb, er;                                            \
                                                                        \
    munit_rand_memory(sizeof(na), (uint8_t*) &na);                      \
    munit_rand_memory(sizeof(nb), (uint8_t*) &nb);                      \
    memcpy(&ea, &na, sizeof(na));                                       \
    memcpy(&eb, &nb, sizeof(nb));                                       \
                                                                        \
    nr = emscripten_##T##_##op(na, nb);                                 \
    er = simde_em_##T##_##op(ea, eb);                                   \
                                                                        \
    simde_assert_##T(nr, ==, er);                                       \
                                                                        \
    return MUNIT_OK;                                                    \
  }

#define SIMDE_JS_DEFINE_TEST_EM_UNARY(T, op)				\
  static MunitResult                                                    \
  test_simde_em_##T##_##op(const MunitParameter params[], void* data) { \
    (void) params;                                                      \
    (void) data;                                                        \
                                                                        \
    T na, nr;                                                           \
    simde_em_##T ea, er;                                                \
                                                                        \
    munit_rand_memory(sizeof(na), (uint8_t*) &na);                      \
    memcpy(&ea, &na, sizeof(na));                                       \
                                                                        \
    nr = emscripten_##T##_##op(na);                                     \
    er = simde_em_##T##_##op(ea);                                       \
                                                                        \
    simde_assert_##T(nr, ==, er);                                       \
                                                                        \
    return MUNIT_OK;                                                    \
  }

#define SIMDE_JS_DEFINE_TEST_EM_COMPARE(T, BT, op)			\
  static MunitResult                                                    \
  test_simde_em_##T##_##op(const MunitParameter params[], void* data) { \
    (void) params;                                                      \
    (void) data;                                                        \
                                                                        \
    T na, nb;                                                           \
    simde_em_##T ea, eb;                                                \
    BT nr;                                                              \
    simde_em_##BT er;                                                   \
                                                                        \
    munit_rand_memory(sizeof(na), (uint8_t*) &na);                      \
    munit_rand_memory(sizeof(nb), (uint8_t*) &nb);                      \
    for (size_t i = 0 ; i < (sizeof(na) / sizeof(na[0])) ; i++)         \
      if (munit_rand_int_range(0, 1))                                   \
        nb[i] = na[i];                                                  \
                                                                        \
    memcpy(&ea, &na, sizeof(na));                                       \
    memcpy(&eb, &nb, sizeof(nb));                                       \
                                                                        \
    nr = emscripten_##T##_##op(na, nb);                                 \
    er = simde_em_##T##_##op(ea, eb);                                   \
                                                                        \
    simde_assert_##T(nr, ==, er);                                       \
                                                                        \
    return MUNIT_OK;                                                    \
  }

#define simde_assert_int32x4(a, op, b) \
  simde_assert_int32v(4, (const int32_t*) &(a), op, (const int32_t*) &(b))

#define simde_assert_bool32x4(a, op, b) \
  simde_assert_uint32vx(4, (const uint32_t*) &(a), op, (const uint32_t*) &(b))

HEDLEY_END_C_DECLS

#endif /* !defined(SIMDE_TESTS_JS_INTERNAL_H) */
