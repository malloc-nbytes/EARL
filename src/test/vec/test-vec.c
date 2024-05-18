#include <string.h>

#include "test-suite.h"
#include "vector.h"
#include "pair.h"

test_errno_t
test_vector_instantiation(void)
{
  struct vector(int) v = vector_create(int);

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_insert_elems(void)
{
  struct vector(int) v = vector_create(int);

  for (int i = 0; i < 10; ++i) {
    vector_append(&v, &i);
  }

  for (int i = 0; i < 10; ++i) {
    TEST_ASSERT_TRUE(vector_unsafe_at(v, i, int) == i);
  }

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_large_elements(void)
{
  struct vector(int) v = vector_create(int);

  for (int i = 0; i < 10000000; ++i) {
    vector_append(&v, &i);
  }

  for (int i = 0; i < 10000000; ++i) {
    TEST_ASSERT_TRUE(vector_unsafe_at(v, i, int) == i);
  }

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_rm_at(void)
{
  struct vector(int) v = vector_create(int);

  for (int i = 0; i < 10; ++i) {
    vector_append(&v, &i);
  }

  vector_rm_at(&v, 0);
  TEST_ASSERT_EQ(*(int *)vector_at(&v, 0), 1);

  vector_rm_at(&v, 8);
  TEST_ASSERT_EQ(*(int *)vector_at(&v, v.len-1), 8);

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_can_hold_strings(void)
{
  struct vector(char **) v = vector_create(char **);

  char *strs[] = {
    "hello world!",
    "foo bar baz",
    "a longer string of a lot of words."
  };

  size_t n = sizeof(strs)/sizeof(*strs);

  for (size_t i = 0; i < n; ++i) {
    vector_append(&v, &strs[i]);
  }

  for (size_t i = 0; i < n; ++i) {
    if (strcmp(vector_unsafe_at(v, i, char *), strs[i]) != 0) {
      return TEST_GENERIC_FAILURE;
    }
  }

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_can_hold_pairs(void)
{
  struct vector(pair(int, int)) v = vector_create(pair(int, int));
}
