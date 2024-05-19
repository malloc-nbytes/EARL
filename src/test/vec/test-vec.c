#include <string.h>

#include "test-suite.h"
#include "utils.h"
#include "vector.h"
#include "pair.h"

int
int_compar(const void *x, const void *y) {
  return *(int *)x > *(int *)y;
}

test_errno_t
test_vector_instantiation(void)
{
  struct vector(int) v = vector_create2(int);

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_insert_elems(void)
{
  struct vector(int) v = vector_create2(int);

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
  struct vector(int) v = vector_create2(int);
  int n = 30000000;

  for (int i = 0; i < n; ++i) {
    vector_append(&v, &i);
  }

  for (int i = 0; i < n; ++i) {
    TEST_ASSERT_TRUE(vector_deref_at(v, i, int) == i);
  }

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vector_rm_at(void)
{
  struct vector(int) v = vector_create2(int);

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
  struct vector(char **) v = vector_create2(char **);

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

  struct vector v = vector_create2(struct pair(any, any));

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

  TEST_ASSERT_MEMEQ(*(char **)pair_fst(p3_ptr), g, strlen(g));
  TEST_ASSERT_MEMEQ(*(char **)pair_snd(p3_ptr), h, strlen(h));

  for (size_t i = 0; i < v.len; ++i) {
    pair_free((struct pair *)vector_at(&v, i));
  }

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vec_can_hold_large_amount_of_pairs(void)
{
  struct vector v = vector_create2(struct pair);
  int n = 2000000;

  for (int i = 0, j = 1; i < n; ++i, ++j) {
    struct pair p = pair_create2(int, int);
    pair_make_unique(&p, &i, &j);
    vector_append(&v, &p);
  }

  for (int i = 0; i < n; ++i) {
    struct pair p = vector_deref_at(v, i, struct pair);
    int a = pair_deref_fst(p, int);
    int b = pair_deref_snd(p, int);

    TEST_ASSERT_EQ(a, i);
    TEST_ASSERT_EQ(b, i+1);

    pair_free(&p);
  }

  vector_free(&v);

  return TEST_OK;
}

test_errno_t
test_vec_can_qsort_with_integers(void)
{
  struct vector v = vector_create2(int);
  size_t n = 3000000;

  for (size_t i = n-1; i > 0; --i) {
    vector_append(&v, &i);
  }

  qsort((void *)vector_asbytes(&v), n, sizeof(int), int_compar);

  for (size_t i = 0; i < n; ++i) {
    TEST_ASSERT_EQ(vector_deref_at(v, i, int), (int)i);
  }

  vector_free(&v);

  return TEST_OK;
}
