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
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 45, true);

    return TEST_OK;
}

test_errno_t test_loops_large_while(void) {
    std::string filepath = "test/sample-input/loops-while.2.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 10000000, true);

    return TEST_OK;
}

test_errno_t test_loops_with_end_as_variable(void) {
    std::string filepath = "test/sample-input/loops-while.3.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 10000000, true);

    return TEST_OK;
}

test_errno_t test_loops_mutating_enumerator_by_5(void) {
    std::string filepath = "test/sample-input/loops-while.4.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 10000000, true);

    return TEST_OK;
}

test_errno_t test_loops_should_not_loop(void) {
    std::string filepath = "test/sample-input/loops-while.5.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 5, true);

    return TEST_OK;
}

test_errno_t test_loops_basic_for(void) {
    std::string filepath = "test/sample-input/loops-for.1.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 49995000, true);

    return TEST_OK;
}

test_errno_t test_loops_large_for(void) {
    std::string filepath = "test/sample-input/loops-for.2.earl";
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 9999999, true);

    return TEST_OK;
}
