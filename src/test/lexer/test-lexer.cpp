#include "test-suite.h"
#include "lexer.h"
#include "token.h"

test_errno_t
test_lexer_symbols(void)
{
  char *filepath = "test/sample-input/lexer-symbols.1.in";
  struct lexer lexer = lex_file(filepath, NULL, 0, "#");

  // ( ) { } ! @ # $ ;
  enum token_type types[] = {
    TOKENTYPE_LPAREN,
    TOKENTYPE_RPAREN,
    TOKENTYPE_LBRACE,
    TOKENTYPE_RBRACE,
    TOKENTYPE_BANG,
    TOKENTYPE_AT,
    TOKENTYPE_HASH,
    TOKENTYPE_DOLLARSIGN,
    TOKENTYPE_SEMICOLON,
  };

  struct token *it;
  int i = 0;
  while ((it = lexer_next(&lexer))->type != TOKENTYPE_EOF) {
    TEST_ASSERT_EQ(it->type, types[i]);
    ++i;
  }

  lexer_free(&lexer);
  return TEST_OK;
}

test_errno_t
test_lexer_identifiers(void)
{
  char *filepath = "test/sample-input/lexer-idents.1.in";
  struct lexer lexer = lex_file(filepath, NULL, 0, "#");

  char *ids[] = {"x", "X", "_x", "x_", "x_x", "xx__", "X_x", "Xx", "xX", "__xX",};

  struct token *it;
  int i = 0;
  while ((it = lexer_next(&lexer))->type != TOKENTYPE_EOF) {
    TEST_ASSERT_EQ(it->type, TOKENTYPE_IDENT);
    TEST_ASSERT_STREQ(it->lexeme, ids[i]);
    ++i;
  }

  lexer_free(&lexer);
  return TEST_OK;
}

test_errno_t
test_lexer_keywords(void)
{
  char *filepath = "test/sample-input/lexer-keywords.1.in";
  char *keywords[] = {"let", "def", "int", "str",};
  struct lexer lexer = lex_file(filepath, keywords, sizeof(keywords)/sizeof(*keywords), "#");

  struct token *it;
  int i = 0;
  while ((it = lexer_next(&lexer))->type != TOKENTYPE_EOF) {
    TEST_ASSERT_EQ(it->type, TOKENTYPE_KEYWORD);
    TEST_ASSERT_STREQ(it->lexeme, keywords[i]);
    ++i;
  }

  lexer_free(&lexer);
  return TEST_OK;
}
