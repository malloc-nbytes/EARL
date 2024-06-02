#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <stdio.h>
#include <cstring>

typedef unsigned test_errno_t;

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define CYAN "\033[0;36m"
#define NOC "\033[0m"

#define TEST_OK                      0
#define TEST_GENERIC_FAILURE         1
#define TEST_ASSERT_TRUE_FAILURE     2
#define TEST_ASSERT_FALSE_FAILURE    3
#define TEST_ASSERT_NOT_NULL_FAILURE 4
#define TEST_ASSERT_NOT_FAILURE      5
#define TEST_ASSERT_EQ_FAILURE       6
#define TEST_ASSERT_NEQ_FAILURE      7
#define TEST_ASSERT_MEMEQ_FAILURE    8

#define TEST_DUMP(msg, d)                                               \
    do {                                                                \
        if (d) {                                                        \
            printf("(" msg ")" " -> " "%s:%s:%d ", __FILE__, __FUNCTION__, __LINE__); \
        }                                                               \
    } while (false)

#define TEST_ASSERT_TRUE(expr, d)            \
    do {                                     \
        if ((expr) != 1) {                   \
            TEST_DUMP(#expr " != true", d);  \
            return TEST_ASSERT_TRUE_FAILURE; \
        }                                    \
    } while (0)

#define TEST_ASSERT_FALSE(expr, d)              \
    do {                                        \
        if ((expr) != 0) {                      \
            TEST_DUMP(#expr " != false", d);    \
            return TEST_ASSERT_FALSE_FAILURE;   \
        }                                       \
    } while (0)

#define TEST_ASSERT_NOT_NULL(expr, d)            \
    do {                                         \
        if ((expr) == nullptr) {                 \
            TEST_DUMP(#expr "!= !NULL", d);      \
            return TEST_ASSERT_NOT_NULL_FAILURE; \
        }                                        \
    } while (0)

#define TEST_ASSERT_NULL(expr, d)               \
    do {                                        \
        if ((expr) != nullptr) {                \
            TEST_DUMP(#expr " != NULL", d);     \
            return TEST_ASSERT_NULL_FAILURE;    \
        }                                       \
    } while (0)

#define TEST_ASSERT_EQ(lhs, rhs, d)             \
    do {                                        \
        if ((lhs) != (rhs)) {                   \
            TEST_DUMP(#lhs " != " #rhs, d);     \
            return TEST_ASSERT_EQ_FAILURE;      \
        }                                       \
    } while (0)

#define TEST_ASSERT_NEQ(lhs, rhs, d)        \
    do {                                    \
        if ((lhs) == (rhs)) {               \
            TEST_DUMP(#lhs " == " #rhs, d); \
            return TEST_ASSERT_EQ_FAILURE;  \
        }                                   \
    } while (0)

#define TEST_ASSERT_MEMEQ(lhs, rhs, bytes)        \
    do {                                          \
        if (std::memcmp(lhs, rhs, bytes) != 0) {  \
            TEST_DUMP(#lhs "!= memory of " #rhs); \
            return TEST_ASSERT_STRNEQ_FAILURE;    \
        }                                         \
    } while (0)

#endif // TEST_SUITE_H
