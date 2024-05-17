#include <stdio.h>

#include "test-suite.h"
#include "vec/test-vec.h"
#include "pair/test-pair.h"

int main(void) {
  int pass = 0, fail = 0;

  RUN_TEST(test_vector_instantiation());
  RUN_TEST(test_vector_insert_elems());
  RUN_TEST(test_vector_large_elements());
  RUN_TEST(test_vector_rm_at());

  RUN_TEST(test_pair_instantiation());
  RUN_TEST(test_pair_same_types_ints());

  printf("TEST SUMMARY\n");
  printf("  PASS: %-5d FAIL: %d\n", pass, fail);

  return 0;
}
