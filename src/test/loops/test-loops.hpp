#ifndef LOOPS_H
#define LOOPS_H

#include "test-suite.hpp"

test_errno_t test_loops_basic_while(void);
test_errno_t test_loops_large_while(void);
test_errno_t test_loops_with_end_as_variable(void);
test_errno_t test_loops_mutating_enumerator_by_5(void);
test_errno_t test_loops_should_not_loop(void);
test_errno_t test_loops_basic_for(void);
test_errno_t test_loops_large_for(void);

#endif // LOOPS_H
