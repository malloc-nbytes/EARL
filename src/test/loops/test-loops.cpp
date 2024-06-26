#include "test-suite.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "common.hpp"
#include "interpreter.hpp"

static std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
static std::vector<std::string> types = COMMON_EARLTY_ASCPL;
static std::string comment = "#";

test_errno_t test_loops_basic_while(void) {
    std::string filepath = "test/sample-input/loops-while.1.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_loops_large_while(void) {
    std::string filepath = "test/sample-input/loops-while.2.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_loops_with_end_as_variable(void) {
    std::string filepath = "test/sample-input/loops-while.3.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_loops_mutating_enumerator_by_5(void) {
    std::string filepath = "test/sample-input/loops-while.4.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_loops_should_not_loop(void) {
    std::string filepath = "test/sample-input/loops-while.5.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_loops_basic_for(void) {
    std::string filepath = "test/sample-input/loops-for.1.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_loops_large_for(void) {
    std::string filepath = "test/sample-input/loops-for.2.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}
