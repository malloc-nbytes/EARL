#include "test-suite.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "common.hpp"
#include "interpreter.hpp"

static std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
static std::vector<std::string> types = COMMON_EARLTY_ASCPL;
static std::string comment = "#";

test_errno_t test_functions_basic_no_return(void) {
    std::string filepath = "test/sample-input/functions-basic-no-return.1.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_functions_return_passed_param(void) {
    std::string filepath = "test/sample-input/functions-return-passed-param.1.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_functions_return_summed_element(void) {
    std::string filepath = "test/sample-input/functions-return-summed-element.1.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_functions_count_evens(void) {
    std::string filepath = "test/sample-input/functions-count-evens.1.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}

test_errno_t test_functions_world_attribute(void) {
    std::string filepath = "test/sample-input/functions-world-attribute.1.earl";
    auto lexer = lex_file(filepath.c_str(), keywords, types, comment);
    auto program = Parser::parse_program(*lexer.get());

    return TEST_OK;
}
