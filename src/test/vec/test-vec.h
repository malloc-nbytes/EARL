#ifndef VEC_TEST_H
#define VEC_TEST_H

#include "test-suite.h"

test_errno_t test_vector_instantiation(void);
test_errno_t test_vector_insert_elems(void);
test_errno_t test_vector_large_elements(void);
test_errno_t test_vector_rm_at(void);
test_errno_t test_vector_can_hold_strings(void);
test_errno_t test_vector_can_hold_pairs(void);
test_errno_t test_vec_can_hold_large_amount_of_pairs(void);

#endif // VEC_TEST_H
