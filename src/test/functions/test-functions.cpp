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
    Lexer lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);
    Interpreter::ExprEvalResult result = Interpreter::interpret(program);

    TEST_ASSERT_EQ(std::any_cast<int>(result.value()), 0, true);

    return TEST_OK;
}
