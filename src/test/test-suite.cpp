#include "functions/test-functions.hpp"
#include "conditionals/test-conditionals.hpp"
#include "loops/test-loops.hpp"
#include "parsers/test-parsers.hpp"
#include "test-suite.hpp"

#include <stdio.h>
#include <time.h>

static int global_pass = 0;
static int global_fail = 0;

// NOTE: RUN_TEST requires that two
// integers namely `pass` & `fail` be
// defined before calling this macro, as it
// will attempt to modify them.
#define RUN_TEST(func)                                          \
    do {                                                        \
        printf("  %s ... ", #func);                             \
        test_errno_t k;                                         \
        if ((k = func) != TEST_OK) {                            \
            ++fail;                                             \
            ++global_fail;                                      \
            printf(RED "FAILED" NOC "(" RED "%d" NOC ")\n", k); \
        } else {                                                \
            ++pass;                                             \
            ++global_pass;                                      \
            printf(GREEN "OK" NOC "\n");                        \
        }                                                       \
    } while (0)

#define RUN_TEST_GROUP(name, ...)                        \
    do {                                                 \
        int pass = 0;                                    \
        int fail = 0;                                    \
        printf(CYAN "[test group] (%s)" NOC "\n", name); \
        __VA_ARGS__                                      \
        if (fail != 0) printf(RED "  BAD" NOC "\n");     \
        else printf(GREEN "  Good" NOC "\n");            \
    } while (0)

#define TEST_SUITE(...)                                                                \
    do {                                                                               \
        printf("--- Running Test Suite ---\n");                                        \
        __VA_ARGS__                                                                    \
        printf("--- Test Suite Summary ---\n");                                        \
        printf(GREEN "PASS: %-5d " RED "FAIL: %d" NOC "\n", global_pass, global_fail); \
    } while (0)

int main(void)
{
    TEST_SUITE({
        RUN_TEST_GROUP("parsers", {
            RUN_TEST(test_parsers_parse_stmt());
            RUN_TEST(test_parsers_parse_stmt_def());
            RUN_TEST(test_parsers_parse_stmt_expr());
            RUN_TEST(test_parsers_parse_stmt_let());
            RUN_TEST(test_parsers_parse_stmt_mut());
            RUN_TEST(test_parsers_parse_expr());
            RUN_TEST(test_parsers_parse_expect_type());
            RUN_TEST(test_parsers_parse_expect_keyword());
            RUN_TEST(test_parsers_parse_expect());
        });

        RUN_TEST_GROUP("loops", {
            RUN_TEST(test_loops_basic_while());
            RUN_TEST(test_loops_large_while());
            RUN_TEST(test_loops_with_end_as_variable());
            RUN_TEST(test_loops_mutating_enumerator_by_5());
            RUN_TEST(test_loops_should_not_loop());
            RUN_TEST(test_loops_basic_for());
            RUN_TEST(test_loops_large_for());
        });

        RUN_TEST_GROUP("conditionals", {
            RUN_TEST(test_conditionals_basic_if());
            RUN_TEST(test_conditionals_basic_if_else_if());
            RUN_TEST(test_conditionals_basic_if_else_if_else());
        });

        RUN_TEST_GROUP("functions", {
            RUN_TEST(test_functions_basic_no_return());
            RUN_TEST(test_functions_return_passed_param());
            RUN_TEST(test_functions_return_summed_element());
            RUN_TEST(test_functions_count_evens());
            RUN_TEST(test_functions_world_attribute());
        });


    });

    return 0;
}
