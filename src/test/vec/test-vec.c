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
    TEST_ASSERT_TRUE(vector_deref_at(v, i, int) == i);
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
    TEST_ASSERT_TRUE(vector_deref_at(v, i, int) == i);
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
    TEST_ASSERT_STREQ(vector_deref_at(v, i, char *), strs[i]);
  }

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_can_hold_pairs(void)
{
  int a = 1, b = 2;
  float c = 3.0, d = 4.0;
  char *g = "hello", *h = "world";

  struct vector v = vector_create(struct pair(any, any));

  struct pair p1 = pair_create2(int, int);
  struct pair p2 = pair_create2(float, float);
  struct pair p3 = pair_create2(char**, char**);

  pair_make_unique(&p1, &a, &b);
  pair_make_unique(&p2, &c, &d);
  pair_make_unique(&p3, &g, &h);

  vector_append(&v, &p1);
  vector_append(&v, &p2);
  vector_append(&v, &p3);

  struct pair *p1_ptr = (struct pair *)vector_at(&v, 0);
  struct pair *p2_ptr = (struct pair *)vector_at(&v, 1);
  struct pair *p3_ptr = (struct pair *)vector_at(&v, 2);

  TEST_ASSERT_EQ(*(int *)pair_fst(p1_ptr), a);
  TEST_ASSERT_EQ(*(int *)pair_snd(p1_ptr), b);

  TEST_ASSERT_EQ(*(float *)pair_fst(p2_ptr), c);
  TEST_ASSERT_EQ(*(float *)pair_snd(p2_ptr), d);

  TEST_ASSERT_STREQ(*(char **)pair_fst(p3_ptr), g);
  TEST_ASSERT_STREQ(*(char **)pair_snd(p3_ptr), h);

  for (size_t i = 0; i < v.len; ++i) {
    pair_free((struct pair *)vector_at(&v, i));
  }

  vector_free(&v);

  return TEST_OK;
}
