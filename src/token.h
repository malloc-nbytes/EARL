#ifndef TOKEN_H
#define TOKEN_H

#include "lexer.h"

struct lexer;

enum token_type {
  TOKENTYPE_LPAREN = 0,
  TOKENTYPE_RPAREN,
  TOKENTYPE_LBRACKET,
  TOKENTYPE_RBRACKET,
  TOKENTYPE_LBRACE,
  TOKENTYPE_RBRACE,
  TOKENTYPE_HASH,
  TOKENTYPE_PERIOD,
  TOKENTYPE_SEMICOLON,
  TOKENTYPE_COMMA,
  TOKENTYPE_GREATERTHAN,
  TOKENTYPE_LESSTHAN,
  TOKENTYPE_EQUALS,
  TOKENTYPE_AMPERSAND,
  TOKENTYPE_ASTERISK,
  TOKENTYPE_PLUS,
  TOKENTYPE_MINUS,
  TOKENTYPE_FORWARDSLASH,
  TOKENTYPE_PIPE,
  TOKENTYPE_CARET,
  TOKENTYPE_QUESTIONMARK,
  TOKENTYPE_BACKWARDSLASH,
  TOKENTYPE_BANG,
  TOKENTYPE_AT,
  TOKENTYPE_DOLLARSIGN,
  TOKENTYPE_PERCENT,
  TOKENTYPE_BACKTICK,
  TOKENTYPE_TILDE,
  TOKENTYPE_COLON,
  TOKENTYPE_SYM_LEN, // DO NOT USE! Used for the length of symbols.

  TOKENTYPE_EOF,
  TOKENTYPE_INTLIT,
  TOKENTYPE_STRLIT,
  TOKENTYPE_CHARLIT,
  TOKENTYPE_IDENT,
  TOKENTYPE_KEYWORD,
};

struct token {
  char *lexeme; // alloc'd
  enum token_type type;
  size_t row;
  size_t col;
  char *fp;
  struct token *next;
};

char *tokentype_to_str(enum token_type type);
struct token *token_alloc(struct lexer *lexer,
                          char *start, size_t len,
                          enum token_type type,
                          size_t row, size_t col, char *fp);

#endif // TOKEN_H
