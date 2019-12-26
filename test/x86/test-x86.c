#include "test-x86.h"
#include "test-x86-internal.h"

#include <bits/types/__FILE.h>
#include <stdio.h>

MunitSuite*
simde_tests_x86_get_suite(void) {
  static MunitSuite children[5];
  static MunitSuite suite = { "/x86", NULL, children, 1, MUNIT_SUITE_OPTION_NONE };
  static const MunitSuite empty = { NULL, NULL, NULL, 1, MUNIT_SUITE_OPTION_NONE };
  
  children[0] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, mmx, native, c)();
  children[1] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, mmx, emul,   c)();
  children[2] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, mmx, native, cpp)();
  children[3] = *SIMDE_TESTS_GENERATE_SYMBOL_FULL(suite, SIMDE_TESTS_CURRENT_ARCH, mmx, emul,   cpp)();
  children[4] = empty;

  printf(":: %s:%d\n", __FILE__, __LINE__);

  return &suite;
}
