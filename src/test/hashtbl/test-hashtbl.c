#include "test-suite.h"
#include "test-hashtbl.h"
#include "hashtbl.h"

/* struct hashtbl hashtbl_create(size_t key_stride, size_t value_stride, */
/*                               unsigned (*hashfunc)(void *key, size_t bytes), */
/*                               int (*keycompar)(void *x, void *y)); */

unsigned
hashfunc(void *k, size_t bytes)
{
  (void)bytes;
  return *(unsigned *)k;
}

int
keycompar(void *k, void *v)
{
  return *(int *)k == *(int *)v;
}

test_errno_t
test_hashtbl_insert(void)
{
  struct hashtbl ht = hashtbl_create(sizeof(int), sizeof(int), hashfunc, keycompar);

  for (int i = 0; i < 10; ++i) {
    int j = i*2;
    hashtbl_insert(&ht, &i, &j);
  }

  return TEST_OK;
}
