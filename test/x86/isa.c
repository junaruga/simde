#define SIMDE_TESTS_CURRENT_ISA xxx
#include "test-x86-internal.h"

static MunitResult
test_simde_mm_foo_bar(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  return MUNIT_OK;
}

static MunitTest tests[] = {
  SIMDE_TESTS_DEFINE_TEST(mm_foo_bar),

  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

HEDLEY_C_DECL MunitSuite* SIMDE_TESTS_GENERATE_SYMBOL(suite)(void) {
  static MunitSuite suite = { (char*) "/" HEDLEY_STRINGIFY(SIMDE_TESTS_CURRENT_ISA), tests, NULL, 1, MUNIT_SUITE_OPTION_NONE };

  return &suite;
}



