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

// File: parser.c
// Description:
//   The implementation of a set number
//   of parsers that will parse different
//   types of statements and expressions.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "notify.h"
#include "token.h"
#include "ast.h"
#include "parser.h"

/********** HELPERS **********/

// Name: parser_expect
//
// Description:
//   Given `lexer` and some expected token type `exp`,
//   will check if the type of the head token in the
//   lexer matches the type `exp`. If they do not
//   match, panic.
struct token *
parser_expect(struct lexer *lexer, enum token_type exp)
{
  struct token *hd = lexer_next(lexer);
  if (hd->type != exp) {
    NOTIFY_ERRARGS(NOTIFY_ERR_SYNTAX, "expect: expected %d but got %d", exp, hd->type);
  }
  return hd;
}

// Name: parser_expect_keyword
//
// Description:
//   Given `lexer` and some expected keyword `keyword`,
//   will check if the type of the head token in the
//   lexer matches the type of keyword AND checks if
//   `keyword` matches lexer->hd->lexeme. If they do not
//   match, panic.
struct token *
parser_expect_keyword(struct lexer *lexer, const char *keyword)
{
  struct token *hd = lexer_next(lexer);
  if (hd->type != TOKENTYPE_KEYWORD || !utils_streq(hd->lexeme, keyword)) {
    NOTIFY_ERRARGS(NOTIFY_ERR_SYNTAX, "expect_keyword: expected keyword %s but got %s", keyword, hd->lexeme);
  }
  return hd;
}

struct token *
parser_expect_type(struct lexer *lexer)
{
  char *keywords[] = COMMON_TY_AS_CPL;
  size_t keywords_len = sizeof(keywords)/sizeof(*keywords);

  struct token *tok = lexer_next(lexer);

  for (size_t i = 0; i < keywords_len; ++i) {
    if (utils_streq(tok->lexeme, keywords[i])) {
      return tok;
    }
  }

  NOTIFY_ERRARGS(NOTIFY_ERR_SYNTAX, "expect_type: expected a type got %s", tok->lexeme);
  return NULL; // unreachable
}

/********** PARSERS **********/

struct expr *
parser_parse_primary_expr(struct lexer *lexer)
{
  assert(0 && "parser_parse_primary_expr: unimplemented");
}

struct expr *
parser_parse_multiplicative_expr(struct lexer *lexer)
{
  assert(0 && "parser_parse_multiplicative_expr: unimplemented");
}

struct expr *
parser_parse_additive_expr(struct lexer *lexer)
{
  assert(0 && "parser_parse_additive_expr: unimplemented");
}

struct expr *
parser_parse_equalitative_expr(struct lexer *lexer)
{
  assert(0 && "parser_parse_equalitative_expr: unimplemented");
}

struct expr *
parser_parse_logical_expr(struct lexer *lexer)
{
  struct expr *lhs = parser_parse_equalitative_expr(lexer);
  while (1) {
    struct token *peeked1 = lexer_peek(lexer, 0);
    struct token *peeked2 = lexer_peek(lexer, 1);
    int nonnull = peeked1 && peeked2;
    int doubleamp = nonnull && (peeked1->type == TOKENTYPE_AMPERSAND && peeked2->type == TOKENTYPE_AMPERSAND);
    int doublepipe = nonnull && (peeked1->type == TOKENTYPE_PIPE && peeked2->type == TOKENTYPE_PIPE);
    if (doubleamp || doublepipe) {
      lexer_discard(lexer);
      lexer_discard(lexer);
      struct expr *rhs = parser_parse_equalitative_expr(lexer);
    }
    // switch (lexer_peek(lexer, 0)->type) {
    //   case TOKENTYPE_AMPERSAND: {
    //   } break;
    //   case TOKENTYPE_PIPE: {
    //   } break;
    //   default:
    //     return lhs;
    // }
  }
}

struct expr *
parser_parse_expr(struct lexer *lexer)
{
  return parser_parse_logical_expr(lexer);
}

// Name: parser_parse_def_stmt_args
//
// Description:
//   Given the syntax of (k1: ty1, k2: ty2,...,kn: tyn)
//   will parse and return a vector of pairs of the id
//   and the associated type i.e.
//     vector[(k1, ty1), (k2, ty2),...,(kn, tyn)].
//
// NOTE: Expects to have the LPAREN ('(') and RPAREN (')')
//   and will consume those.
struct vector(struct pair(struct token *id, struct token *type))
parser_parse_stmt_def_args(struct lexer *lexer)
{
  (void)parser_expect(lexer, TOKENTYPE_LPAREN);

  struct vector(pair(token *id, token *type)) args =
    vector_create2(struct pair);

  // Case of no arguments.
  if (lexer_peek(lexer, 0)->type == TOKENTYPE_RPAREN) {
    return args;
  }

  while (1) {
    struct token *id = parser_expect(lexer, TOKENTYPE_IDENT);
    (void)parser_expect(lexer, TOKENTYPE_COLON);
    struct token *type = parser_expect_type(lexer);

    struct pair p = pair_create2(struct token *, struct token *);
    pair_make_unique(&p, id, type);
    vector_append(&args, &p);

    if (lexer_peek(lexer, 0)->type != TOKENTYPE_COMMA) {
      break;
    }
    (void)parser_expect(lexer, TOKENTYPE_COMMA);
  }

  (void)parser_expect(lexer, TOKENTYPE_RPAREN);
  return args;
}

// Name: parser_parse_stmt_block
//
// Description:
//   Parses a block statement, namely
//   {stmt; stmt; stmt; ..., stmt;}
//
// Note:
//   Expects to have opening and closing
//   curly braces `{}` and will consume those.
struct stmt_block *
parser_parse_stmt_block(struct lexer *lexer)
{
  (void)parser_expect(lexer, TOKENTYPE_LBRACE);

  struct vector stmts = vector_create2(struct stmt *);

  while (1) {
    if (lexer_peek(lexer, 0)->type == TOKENTYPE_RBRACE) {
      break;
    }
    struct stmt *stmt = parser_parse_stmt(lexer);
    vector_append(&stmts, stmt);
  }

  (void)parser_expect(lexer, TOKENTYPE_LBRACE);

  return stmt_block_alloc(stmts);
}

struct stmt_def *
parser_parse_stmt_def(struct lexer *lexer)
{
  // def
  (void)parser_expect_keyword(lexer, COMMON_KW_DEF);

  // identifier
  struct token *id = parser_expect(lexer, TOKENTYPE_IDENT);

  // (...)
  struct vector(struct pair(struct token *id, struct token *type)) args
    = parser_parse_stmt_def_args(lexer);

  // ->
  (void)parser_expect(lexer, TOKENTYPE_MINUS);
  (void)parser_expect(lexer, TOKENTYPE_GREATERTHAN);

  // type
  struct token *rettype = parser_expect_type(lexer);

  // { ... }
  struct stmt_block *block = parser_parse_stmt_block(lexer);

  return stmt_def_alloc(id, args, rettype, block);
}

// Name: parser_parse_stmt_let
//
// Description:
//   Parses a let statement in the form of
//     let id: type = expr;
//
// Note:
//   Expects the first token to be `let` and
//   expects the end to have a semicolon and
//   will consume these.
struct stmt_let *
parser_parse_stmt_let(struct lexer *lexer)
{
  (void)parser_expect_keyword(lexer, COMMON_KW_LET);
  struct token *id = parser_expect(lexer, TOKENTYPE_IDENT);
  (void)parser_expect(lexer, TOKENTYPE_COLON);
  struct token *type = parser_expect_type(lexer);
  (void)parser_expect(lexer, TOKENTYPE_EQUALS);
  struct expr *expr = parser_parse_expr(lexer);
  (void)parser_expect(lexer, TOKENTYPE_SEMICOLON);
  return stmt_let_alloc(id, type, expr);
}

struct stmt *
parser_parse_stmt(struct lexer *lexer)
{
  switch (lexer_peek(lexer, 0)->type) {
  case TOKENTYPE_KEYWORD: {
    struct token *tok = lexer_next(lexer);
    if (utils_streq(tok->lexeme, COMMON_KW_DEF)) {
      return stmt_alloc(STMT_TYPE_DEF, parser_parse_stmt_def(lexer));
    }
    else if (utils_streq(tok->lexeme, COMMON_KW_LET)) {
      return stmt_alloc(STMT_TYPE_LET, parser_parse_stmt_let(lexer));
    }
  } break;

  case TOKENTYPE_IDENT: {
    assert(0 && "parser_parse_stmt: TOKENTYPE_IDENT unimplemented");
  } break;

  default:
    NOTIFY_ERRARGS(ERR_FATAL, "parse_stmt found an unkown statement of type ID (%d).", lexer->hd->type);
  }

  assert(0 && "parser_parse_stmt: ending is unimplemented");
  return NULL;
}

struct vector(struct stmt *)
parser_parse_stmts(struct lexer *lexer)
{
  struct vector stmts = vector_create2(struct stmt *);

  struct token *curtok = NULL;
  while ((curtok = lexer_next(lexer)) != NULL) {
    if (curtok->type == TOKENTYPE_EOF) break;

    switch (curtok->type) {
      case TOKENTYPE_KEYWORD: {
        if (utils_streq(curtok->lexeme, COMMON_KW_LET)) {
          ;
        }
        else if (utils_streq(curtok->lexeme, COMMON_KW_DEF)) {
          ;
        }
        break;
      }
      case TOKENTYPE_IDENT: {
        break;
      }
      default: {
        NOTIFY_ERRARGS(ERR_FATAL, "parse_stmts found an unkown statement of type ID (%d).", curtok->type);
      }
    }
  }

  return stmts;
}

/********** Entrypoint **********/

struct program
parser_parse(struct lexer *lexer)
{
  return (struct program) {
    .stmts = parser_parse_stmts(lexer),
  };
}
