#include "test-suite.h"
#include "pair.h"

test_errno_t
test_pair_instantiation(void)
{
  struct pair(int, int) p = pair_create2(int, int);

  pair_free(&p);

  return TEST_OK;
}

test_errno_t
test_pair_same_types_ints(void)
{
  struct pair(int, int) p = pair_create2(int, int);

  int a = 1, b = 2;

  pair_make_unique(&p, &a, &b);

  TEST_ASSERT_EQ(*(int *)pair_fst(&p), a);
  TEST_ASSERT_EQ(*(int *)pair_snd(&p), b);

  TEST_ASSERT_EQ(pair_deref_fst(p, int), a);
  TEST_ASSERT_EQ(pair_deref_snd(p, int), b);

  pair_free(&p);

  return TEST_OK;
}

// TODO: Test that a pair is not empty
