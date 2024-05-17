#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <assert.h>
#include <stdio.h>

typedef int test_errno_t;

#define TEST_PASS                    0
#define TEST_GENERIC_FAILURE         1
#define TEST_ASSERT_TRUE_FAILURE     2
#define TEST_ASSERT_FALSE_FAILURE    3
#define TEST_ASSERT_NOT_NULL_FAILURE 4
#define TEST_ASSERT_NOT_FAILURE      5

#define TEST_ASSERT_TRUE(expr)                  \
  do {                                          \
    if ((expr) != 1) {                          \
      return TEST_ASSERT_TRUE_FAILURE;          \
    }                                           \
  } while (0)

#define TEST_ASSERT_FALSE(expr) \
  do {                                          \
    if ((expr) != 0) {                          \
      return TEST_ASSERT_FALSE_FAILURE;         \
    }                                           \
  } while (0)

#define TEST_ASSERT_NOT_NULL(expr)              \
  do {                                          \
    if ((expr) == NULL) {                       \
      return TEST_ASSERT_NOT_NULL_FAILURE;      \
    }                                           \
  } while (0)

#define TEST_ASSERT_NULL(expr)                  \
  do {                                          \
    if ((expr) != NULL) {                       \
      return TEST_ASSERT_NULL_FAILURE;          \
    }                                           \
  } while (0)

#define RUN_TEST(func)                          \
  do {                                          \
    printf("[test] %s ... ", #func);            \
    test_errno_t k;                             \
    if ((k = func) != TEST_PASS) {              \
      printf("FAILED (%d)\n", k);               \
    } else {                                    \
      printf("ok\n");                           \
    }                                           \
  } while (0)

#endif // TEST_SUITE_H
