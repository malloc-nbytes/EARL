// MIT License

// Copyright (c) 2023 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef TOKEN_H
#define TOKEN_H

#include "lexer.h"

struct lexer;

enum token_type {
  // Single character
  TOKENTYPE_LPAREN,
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

  // Multi character
  TOKENTYPE_DOUBLE_AMPERSAND,
  TOKENTYPE_DOUBLE_PIPE,
  TOKENTYPE_GREATERTHAN_EQUALS,
  TOKENTYPE_LESSTHAN_EQUALS,
  TOKENTYPE_DOUBLE_EQUALS,
  TOKENTYPE_BANG_EQUALS,
  TOKENTYPE_PLUS_EQUALS,
  TOKENTYPE_MINUS_EQUALS,
  TOKENTYPE_ASTERISK_EQUALS,
  TOKENTYPE_FORWARDSLASH_EQUALS,
  TOKENTYPE_PERCENT_EQUALS,

  // Other
  TOKENTYPE_EOF,
  TOKENTYPE_INTLIT,
  TOKENTYPE_STRLIT,
  TOKENTYPE_CHARLIT,
  TOKENTYPE_IDENT,
  TOKENTYPE_KEYWORD,
  TOKENTYPE_TOTAL_LEN
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

int token_is_vartype(char *s);

#endif // TOKEN_H
