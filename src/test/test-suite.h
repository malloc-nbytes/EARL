#indef TEST_SUITE_H
#define TEST_SUITE_H

#include <assert.h>
#include <stdio.h>

typedef int test_errno_t;

#define TEST_OK                      0
#define TEST_GENERIC_FAILURE         1
#define TEST_ASSERT_TRUE_FAILURE     2
#define TEST_ASSERT_FALSE_FAILURE    3
#define TEST_ASSERT_NOT_NULL_FAILURE 4
#define TEST_ASSERT_NOT_FAILURE      5
#define TEST_ASSERT_EQ_FAILURE       6
#define TEST_ASSERT_NEQ_FAILURE      7
#define TEST_ASSERT_STREQ_FAILURE    8
#define TEST_ASSERT_STRNEQ_FAILURE   9

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

#define TEST_ASSERT_EQ(lhs, rhs)                \
  do {                                          \
    if ((lhs) != (rhs)) {                       \
      return TEST_ASSERT_EQ_FAILURE;            \
    }                                           \
  } while (0)

#define TEST_ASSERT_NEQ(lhs, rhs)               \
  do {                                          \
    if ((lhs) == (rhs)) {                       \
      return TEST_ASSERT_EQ_FAILURE;            \
    }                                           \
  } while (0)

#endif // TEST_SUITE_H
