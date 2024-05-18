#include <stdio.h>

#include "test-suite.h"
#include "vec/test-vec.h"
#include "pair/test-pair.h"

// NOTE: RUN_TEST requires that two
// integers namely `pass` & `fail` be
// defined before calling this macro, as it
// will attempt to modify them.
#define RUN_TEST(func)                          \
  do {                                          \
    printf("[test] %s ... ", #func);            \
    test_errno_t k;                             \
    if ((k = func) != TEST_OK) {                \
      ++fail;                                   \
      printf("FAILED (%d)\n", k);               \
    } else {                                    \
      ++pass;                                   \
      printf("ok\n");                           \
    }                                           \
  } while (0)

int main(void) {
  int pass = 0, fail = 0;

  RUN_TEST(test_vector_instantiation());
  RUN_TEST(test_vector_insert_elems());
  RUN_TEST(test_vector_large_elements());
  RUN_TEST(test_vector_rm_at());
  RUN_TEST(test_vector_can_hold_strings());

  RUN_TEST(test_pair_instantiation());
  RUN_TEST(test_pair_same_types_ints());

  printf("TEST SUMMARY\n");
  printf("  PASS: %-5d FAIL: %d\n", pass, fail);

  return 0;
}
