#include <assert.h>

#include "test-suite.h"
#include "vector.h"

test_errno_t
test_vector_insert_elems(void)
{
  struct vector(int) v = vector_create(int);

  for (int i = 10; i < 20; ++i) {
    vector_append(&v, &i);
  }

  for (int i = 0; i < 10; ++i) {
    TEST_ASSERT_TRUE(vector_unsafe_at(v, i, int) == i);
  }

  return TEST_PASS;
}

int main(void)
{
  RUN_TEST(test_vector_insert_elems());

  return 0;
}
