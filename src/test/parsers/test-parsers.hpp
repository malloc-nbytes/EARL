#ifndef PARSERS_H
#define PARSERS_H

#include "token.hpp"
#include "lexer.hpp"
#include "test-suite.hpp"

test_errno_t test_parsers_parse_stmt(void);
test_errno_t test_parsers_parse_stmt_def(void);
test_errno_t test_parsers_parse_stmt_expr(void);
test_errno_t test_parsers_parse_stmt_let(void);
test_errno_t test_parsers_parse_stmt_mut(void);
test_errno_t test_parsers_parse_expr(void);
test_errno_t test_parsers_parse_expect_type(void);
test_errno_t test_parsers_parse_expect_keyword(void);
test_errno_t test_parsers_parse_expect(void);

#endif // PARSERS_H
