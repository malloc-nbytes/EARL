#include "test-suite.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "vector.h"
#include "pair.h"

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

test_errno_t
test_parsers_expect2(void)
{
  char *filepath = "test/sample-input/expect.2.in";
  struct lexer lexer = lex_file(filepath, NULL, 0, "#");

  struct token *intlit1 = parser_expect(&lexer, TOKENTYPE_INTLIT);
  struct token *strlit1 = parser_expect(&lexer, TOKENTYPE_STRLIT);
  struct token *id1 = parser_expect(&lexer, TOKENTYPE_IDENT);
  struct token *intlit2 = parser_expect(&lexer, TOKENTYPE_INTLIT);
  struct token *id2 = parser_expect(&lexer, TOKENTYPE_IDENT);
  struct token *id3 = parser_expect(&lexer, TOKENTYPE_IDENT);
  struct token *id4 = parser_expect(&lexer, TOKENTYPE_IDENT);

  TEST_ASSERT_EQ(intlit1->type, TOKENTYPE_INTLIT);
  TEST_ASSERT_EQ(strlit1->type, TOKENTYPE_STRLIT);
  TEST_ASSERT_EQ(id1->type, TOKENTYPE_IDENT);
  TEST_ASSERT_EQ(intlit2->type, TOKENTYPE_INTLIT);
  TEST_ASSERT_EQ(id2->type, TOKENTYPE_IDENT);
  TEST_ASSERT_EQ(id3->type, TOKENTYPE_IDENT);
  TEST_ASSERT_EQ(id4->type, TOKENTYPE_IDENT);

  lexer_free(&lexer);

  return TEST_OK;

}

test_errno_t
test_parsers_expectkeyword1(void)
{
  char *filepath = "test/sample-input/expect-keyword.1.in";
  char *keywords[] = {
    "let",
    "def",
  };
  struct lexer lexer = lex_file(filepath, keywords, 2, "#");

  struct token *let = parser_expect_keyword(&lexer, COMMON_KW_LET);
  struct token *def = parser_expect_keyword(&lexer, COMMON_KW_DEF);

  TEST_ASSERT_STREQ(let->lexeme, COMMON_KW_LET);
  TEST_ASSERT_STREQ(def->lexeme, COMMON_KW_DEF);

  lexer_free(&lexer);

  return TEST_OK;
}

test_errno_t
test_parsers_parser_parse_def_stmt_args_can_parse_correctly(void)
{
  char *filepath = "test/sample-input/def-stmt-args-tuple.1.in";
  struct lexer lexer = lex_file(filepath, NULL, 0, "#");

  struct vector v = parser_parse_def_stmt_args(&lexer);

  lexer_free(&lexer);

  vector_free(&v);

  return TEST_OK;
}
