#include <stdio.h>
#include <time.h>

#include "test-suite.h"
#include "vec/test-vec.h"
#include "pair/test-pair.h"

int
main(void) {
  int pass = 0;
  int fail = 0;

  time_t start, end;
  double elapsed;

  start = time(NULL);
  RUN_TEST(test_vector_instantiation());
  RUN_TEST(test_vector_insert_elems());
  RUN_TEST(test_vector_large_elements());
  RUN_TEST(test_vector_rm_at());
  RUN_TEST(test_vector_can_hold_strings());

  RUN_TEST(test_pair_instantiation());
  RUN_TEST(test_pair_same_types_ints());
  end = time(NULL);

  elapsed = difftime(end, start);

  printf("[test] total time: %.5f\n", elapsed);
  printf("TEST SUMMARY\n");
  printf("  PASS: %-5d FAIL: %d\n", pass, fail);

  return 0;
}
