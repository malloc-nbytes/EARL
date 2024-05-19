#include <stdio.h>
#include <time.h>

#include "test-suite.h"
#include "vec/test-vec.h"
#include "pair/test-pair.h"
#include "parsers/test-parsers.h"

// NOTE: RUN_TEST requires that two
// integers namely `pass` & `fail` be
// defined before calling this macro, as it
// will attempt to modify them.
#define RUN_TEST(func)                                                  \
  do {                                                                  \
    printf("[test] %s ... ", #func);                                    \
    test_errno_t k;                                                     \
    if ((k = func) != TEST_OK) {                                        \
      ++fail;                                                           \
      printf("\033[0;31mFAILED\033[0m (\033[031m%d\033[0m)\n", k);      \
    } else {                                                            \
      ++pass;                                                           \
      printf("\033[0;32mOK\033[0m\n");                                  \
    }                                                                   \
  } while (0)

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
  RUN_TEST(test_vector_can_hold_pairs());
  RUN_TEST(test_vec_can_hold_large_amount_of_pairs());

  RUN_TEST(test_pair_instantiation());
  RUN_TEST(test_pair_same_types_ints());

  RUN_TEST(test_parsers_expect1());
  RUN_TEST(test_parsers_expect2());
  RUN_TEST(test_parsers_expectkeyword1());
  end = time(NULL);

  elapsed = difftime(end, start);

  printf("[test] total time: %.5f\n", elapsed);
  printf("TEST SUMMARY\n");
  printf("  PASS: %-5d FAIL: %d\n", pass, fail);

  return 0;
}
