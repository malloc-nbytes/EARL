#include <iostream>

#include "test-suite.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "common.hpp"
#include "interpreter.hpp"

test_errno_t test_loops_basic_while(void) {
    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types = COMMON_EARLTY_ASCPL;
    std::string comment = "#";

    std::string filepath = "test/sample-input/loops-while.1.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 45, true);

    return TEST_OK;
}

test_errno_t test_loops_large_while(void) {
    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types = COMMON_EARLTY_ASCPL;
    std::string comment = "#";

    std::string filepath = "test/sample-input/loops-while.2.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 10000000, true);

    return TEST_OK;
}
