#include "test-suite.h"
#include "vector.h"

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
