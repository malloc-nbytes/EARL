#include "test-suite.h"
#include "hashtbl.h"
#include "test-hashtbl.h"

// Add test implementations here

unsigned bad_hash(void *key, size_t bytes) {
  return *(unsigned*)key; 
}

int keycompar (void *x, void *y) { 
  // assert(x && y && "Good pointers");
  // return *(int*)x == *(int*)y;
  return 1; 
}

test_errno_t
test_hashtbl_instantiation(void)
{
  struct hashtbl ht = hashtbl_create(sizeof(int), sizeof(int),
               bad_hash,
               keycompar);

  int * p = NULL; 
  for(int i = 0; i < 3; ++i)
  {
    int val = 2 * i + 1;
    hashtbl_insert(&ht, &i, &val);  
  }

  return TEST_OK;
}