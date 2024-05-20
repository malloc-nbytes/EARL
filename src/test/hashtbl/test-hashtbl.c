#include <string.h>

#include "test-suite.h"
#include "test-hashtbl.h"
#include "utils.h"
#include "hashtbl.h"

/* struct hashtbl hashtbl_create(size_t key_stride, size_t value_stride, */
/*                               unsigned (*hashfunc)(void *key, size_t bytes), */
/*                               int (*keycompar)(void *x, void *y)); */

unsigned
hashfunc_int(void *k, size_t bytes)
{
  (void)bytes;
  return *(unsigned *)k;
}

int
keycompar_int(void *k, void *v)
{
  return *(int *)k == *(int *)v;
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
  return utils_streq((char *)k, (char *)v);
}

test_errno_t
test_hashtbl_insert(void)
{
  struct hashtbl ht = hashtbl_create(sizeof(int), sizeof(int), hashfunc_int, keycompar_int);

  for (int i = 0; i < 5; ++i) {
    int j = i*2;
    hashtbl_insert(&ht, &i, &j);
  }

  int val = 1;
  int val2 = 2;
  TEST_ASSERT_EQ(2, hashtbl_deref_get(ht, val, int));
  TEST_ASSERT_EQ(4, *(int *)hashtbl_get(&ht, &val2));

  hashtbl_free(&ht);

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

  hashtbl_free(&ht);

  return TEST_OK;
}

