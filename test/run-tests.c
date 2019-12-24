#include "munit/munit.h"

#include "x86/test-x86.h"

int main(int argc, char* argv[argc + 1]) {
  static MunitSuite suite = { "", NULL, NULL, 1, MUNIT_SUITE_OPTION_NONE };
  MunitSuite suites[] = {
    *simde_tests_x86_get_suite(),
    { NULL, NULL, NULL, 1, MUNIT_SUITE_OPTION_NONE }
  };

  suite.suites = suites;

  return munit_suite_main(&suite, NULL, argc, argv);
}
