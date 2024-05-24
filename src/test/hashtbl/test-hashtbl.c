#include <string.h>

#include "test-suite.h"
#include "utils.h"
#include "hashtbl.h"

unsigned
hashfunc_int(void *k, size_t bytes)
{
  (void)bytes;
  return *(unsigned *)k;
}

int
keycompar_int(void *k, void *v)
{
  return (int *)k == (int *)v;
}

unsigned
hashfunc_str(void *x, size_t bytes)
{
  (void)bytes;
  return strlen((char *)x);
}

int
keycompar_str(void *k, void *v)
{
  return strcmp(*(char **)k, *(char **)v);
}

test_errno_t
test_hashtbl_insert(void)
{
  struct hashtbl ht = hashtbl_create2(int *, int, hashfunc_int, keycompar_int);

  for (int i = 0; i < 5; ++i) {
    int j = i*2;
    hashtbl_insert(&ht, &i, &j);
  }

  int val = 1;
  int val2 = 2;

  TEST_ASSERT_EQ(2, *(int *)hashtbl_get(&ht, &val));
  TEST_ASSERT_EQ(2, hashtbl_deref_get(ht, val, int));
  TEST_ASSERT_EQ(4, *(int *)hashtbl_get(&ht, &val2));

  /* hashtbl_free(&ht); */

  return TEST_OK;
}

test_errno_t
test_hashtbl_insert_with_strs(void)
{
  char *doubleamp_key = "&&";
  char *doublepipe_key = "||";
  char *ge_key = ">=";
  char *lt_key = "<=";
  char *doubleeq_key = "==";

  int doubleamp_val = 1;
  int doublepipe_val = 2;
  int ge_val = 3;
  int lt_val = 4;
  int doubleeq_val = 5;

  struct hashtbl ht = hashtbl_create2(char **, int, hashfunc_str, keycompar_str);

  hashtbl_insert(&ht, &doubleamp_key, &doubleamp_val);
  hashtbl_insert(&ht, &doublepipe_key, &doublepipe_val);
  hashtbl_insert(&ht, &ge_key, &ge_val);
  hashtbl_insert(&ht, &lt_key, &lt_val);
  hashtbl_insert(&ht, &doubleeq_key, &doubleeq_val);

  TEST_ASSERT_EQ(hashtbl_deref_get(ht, doubleamp_key, int), doubleamp_val);
  TEST_ASSERT_EQ(hashtbl_deref_get(ht, doublepipe_key, int), doublepipe_val);
  TEST_ASSERT_EQ(hashtbl_deref_get(ht, ge_key, int), ge_val);
  TEST_ASSERT_EQ(hashtbl_deref_get(ht, lt_key, int), lt_val);
  TEST_ASSERT_EQ(hashtbl_deref_get(ht, doubleeq_key, int), doubleeq_val);

  /* hashtbl_free(&ht); */

  return TEST_OK;
}

test_errno_t
test_hashtbl_insert_compound_literals(void)
{
  struct hashtbl ht = hashtbl_create2(char **, int, hashfunc_str, keycompar_str);

  hashtbl_insert(&ht, CPL(char *, "&&"),          CPL(int, 1));
  hashtbl_insert(&ht, CPL(char *, "hello world"), CPL(int, 2));
  hashtbl_insert(&ht, CPL(char *, "||"),          CPL(int, 3));

  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "&&")), 1);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "hello world")), 2);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "||")), 3);

  /* hashtbl_free(&ht); */
  return TEST_OK;
}

test_errno_t
test_hashtbl_insert_inplace(void)
{
  struct hashtbl ht = hashtbl_create2(char **, int, hashfunc_str, keycompar_str);

  hashtbl_insert_inplace(ht, char *, "hello world",     int, 1);
  hashtbl_insert_inplace(ht, char *, "foo bar baz",     int, 2);
  hashtbl_insert_inplace(ht, char *, "test string",     int, 3);
  hashtbl_insert_inplace(ht, char *, "string number 4", int, 4);
  hashtbl_insert_inplace(ht, char *, "the value is 5",  int, 5);
  hashtbl_insert_inplace(ht, char *, "a",               int, 6);
  hashtbl_insert_inplace(ht, char *, "ab",              int, 7);
  hashtbl_insert_inplace(ht, char *, "abc",             int, 8);

  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "hello world")),     1);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "foo bar baz")),     2);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "test string")),     3);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "string number 4")), 4);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "the value is 5")),  5);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "a")),               6);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "ab")),              7);
  TEST_ASSERT_EQ(*(int *)hashtbl_get(&ht, CPL(char *, "abc")),             8);

  /* hashtbl_free(&ht); */
  return TEST_OK;
}
