#include "sample/test-sample.hpp"
#include "test-suite.hpp"

#include <stdio.h>
#include <time.h>

static int global_pass = 0;
static int global_fail = 0;

// NOTE: RUN_TEST requires that two
// integers namely `pass` & `fail` be
// defined before calling this macro, as it
// will attempt to modify them.
#define RUN_TEST(func)                                             \
  do {                                                             \
    printf("  %s ... ", #func);                                    \
    test_errno_t k;                                                \
    if ((k = func) != TEST_OK) {                                   \
      ++fail;                                                      \
      ++global_fail;                                               \
      printf("\033[0;31mFAILED\033[0m (\033[031m%d\033[0m)\n", k); \
    } else {                                                       \
      ++pass;                                                      \
      ++global_pass;                                               \
      printf("\033[0;32mOK\033[0m\n");                             \
    }                                                              \
  } while (0)

#define RUN_TEST_GROUP(name, ...)                                       \
  do {                                                                  \
    int pass = 0;                                                       \
    int fail = 0;                                                       \
    printf("\033[0;36m[test group] (%s)\033[0m\n", name);               \
    __VA_ARGS__                                                         \
    if (fail != 0) printf("  \033[0;31mBAD\033[0m\n");                  \
    else printf("  \033[0;32mGood\033[0m\n");                           \
  } while (0)

#define TEST_SUITE(...)                                                                   \
  do {                                                                                    \
    printf("--- Running Test Suite ---\n");                                               \
    __VA_ARGS__                                                                           \
    printf("--- Test Suite Summary ---\n");                                               \
    printf("\033[0;32mPASS: %-5d \033[0;31mFAIL: %d\033[0m\n", global_pass, global_fail); \
  } while (0)

int
main(void)
{
  return 0;
}
