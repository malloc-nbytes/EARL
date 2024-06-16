#include "test-suite.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "common.hpp"
#include "interpreter.hpp"

static std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
static std::vector<std::string> types = COMMON_EARLTY_ASCPL;
static std::string comment = "#";

test_errno_t test_conditionals_basic_if(void) {
    std::string filepath = "test/sample-input/conditionals-if.1.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);

    return TEST_OK;
}

test_errno_t test_conditionals_basic_if_else_if(void) {
    std::string filepath = "test/sample-input/conditionals-if.2.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);

    return TEST_OK;
}

test_errno_t test_conditionals_basic_if_else_if_else(void) {
    std::string filepath = "test/sample-input/conditionals-if.3.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);

    return TEST_OK;
}

