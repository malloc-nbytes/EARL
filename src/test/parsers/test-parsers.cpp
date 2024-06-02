#include <vector>

#include "test-parsers.hpp"
#include "test-suite.hpp"

#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "common.hpp"

std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
std::vector<std::string> types = COMMON_EARLTY_ASCPL;
std::string comment = "#";

test_errno_t test_parsers_parse_stmt(void) {
    std::string filepath = "test/sample-input/parse-stmt.1.in";

    StmtType tys[] = {
        StmtType::Let,
        StmtType::Let,
        StmtType::Def,
        StmtType::Def,
        StmtType::Def,
        StmtType::Stmt_Expr,
    };

    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);

    int i = 0;
    for (auto &stmt : program.m_stmts) {
        TEST_ASSERT_EQ(stmt->stmt_type(), tys[i], true);
        ++i;
    }

    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_def(void) {
    std::string filepath = std::string("test/sample-input/parse_stmt_def.1.in");
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);

    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_expr(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_let(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_mut(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expr(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expect_type(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expect_keyword(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expect(void) {
    return TEST_OK;
}

