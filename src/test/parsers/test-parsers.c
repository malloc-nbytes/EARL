#include "test-suite.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

test_errno_t
test_parsers_expect1(void)
{
  char *filepath = "test/sample-input/expect.1.in";
  struct lexer lexer = lex_file(filepath, NULL, 0, "#");

  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_LPAREN)->lexeme, "(");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_RPAREN)->lexeme, ")");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_LBRACE)->lexeme, "{");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_RBRACE)->lexeme, "}");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_COLON)->lexeme, ":");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_MINUS)->lexeme, "-");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_GREATERTHAN)->lexeme, ">");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_LESSTHAN)->lexeme, "<");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_SEMICOLON)->lexeme, ";");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_TILDE)->lexeme, "~");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_PERCENT)->lexeme, "%");
  TEST_ASSERT_STREQ(parser_expect(&lexer, TOKENTYPE_EQUALS)->lexeme, "=");

  lexer_free(&lexer);

  return TEST_OK;
}

