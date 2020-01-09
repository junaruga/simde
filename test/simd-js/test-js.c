#include "test-js.h"
#include "test-js-internal.h"

#include <stdio.h>

#define SET_CHILDREN_FOR_ARCH(isax) \
  children[i++] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, native, c)(); \
  children[i++] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, emul,   c)(); \
  children[i++] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, native, cpp)(); \
  children[i++] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, isax, emul,   cpp)()

MunitSuite*
simde_tests_js_get_suite(void) {
  static MunitSuite children[(1 * 4) + 1];
  static MunitSuite suite = { "/js", NULL, children, 1, MUNIT_SUITE_OPTION_NONE };
  static const MunitSuite empty = { NULL, NULL, NULL, 1, MUNIT_SUITE_OPTION_NONE };

  size_t i = 0;
  
  SET_CHILDREN_FOR_ARCH(int32x4);

  children[i++] = empty;

  munit_assert_size(i, ==, sizeof(children) / sizeof(children[0]));

  return &suite;
}
