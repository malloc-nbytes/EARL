#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "test-suite.hpp"

test_errno_t test_functions_basic_no_return(void);
test_errno_t test_functions_return_passed_param(void);
test_errno_t test_functions_return_summed_element(void);
test_errno_t test_functions_count_evens(void);

#endif // FUNCTIONS_H
