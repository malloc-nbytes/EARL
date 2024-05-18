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

#include <stdlib.h>

#include "utils.h"
#include "notify.h"
#include "ast.h"

struct stmt *
stmt_alloc(enum stmt_type type, void *stmt)
{
  struct stmt *s = utils_safe_malloc(sizeof(struct stmt));
  s->type = type;

  switch (type) {
  case STMT_TYPE_DEF:
    s->stmt.def = (struct stmt_def *)stmt;
    break;
  case STMT_TYPE_LET:
    s->stmt.let = (struct stmt_let *)stmt;
    break;
  case STMT_TYPE_BLOCK:
    s->stmt.block = (struct stmt_block *)stmt;
    break;
  case STMT_TYPE_MUT:
    s->stmt.mut = (struct stmt_mut *)stmt;
    break;
  case STMT_TYPE_STMT_EXPR:
    s->stmt.expr = (struct expr *)stmt;
    break;
  default:
     NOTIFY_ERRARGS(NOTIFY_ERR_UNKNOWN, "stmt_alloc: unkown type: %d", type);
  }

  return s;
}

struct stmt_def *
stmt_def_alloc(struct token *id,
               struct vector(struct pair(struct token *id, struct token *type)) args,
               struct token *rettype,
               struct stmt_block *block)
{
  struct stmt_def *s = utils_safe_malloc(sizeof(struct stmt_def));
  s->id = id;
  s->args = args;
  s->rettype = rettype;
  s->block = block;
  return s;
}

struct stmt_let *
stmt_let_alloc(struct token *id, struct token *type, struct expr *expr)
{
  struct stmt_let *s = utils_safe_malloc(sizeof(struct stmt_let));
  s->id = id;
  s->type = type;
  s->expr = expr;
  return s;
}

struct stmt_block *
stmt_block_alloc(struct vector(struct stmt *) stmts)
{
  struct stmt_block *s = utils_safe_malloc(sizeof(struct stmt_block));
  s->stmts = stmts;
  return s;
}

struct stmt_mut *
stmt_mut_alloc(struct expr *lhs, struct token *op, struct expr *rhs)
{
  struct stmt_mut *s = utils_safe_malloc(sizeof(struct stmt_mut));
  s->left = lhs;
  s->op = op;
  s->right = rhs;
  return s;
}

struct expr_term *
expr_term_alloc(enum expr_term_type type, struct token *term)
{
  struct expr_term *e = utils_safe_malloc(sizeof(struct expr_term));
  e->type = type;
  e->term = term;
  return e;
}

struct expr_binary *
expr_binary_alloc(struct expr *lhs, struct token *op, struct expr *rhs)
{
  struct expr_binary *e = utils_safe_malloc(sizeof(struct expr_binary));
  e->lhs = lhs;
  e->op = op;
  e->rhs = rhs;
  return e;
}

struct expr *
expr_alloc(enum expr_type type, void *expr)
{
  struct expr *e = utils_safe_malloc(sizeof(struct expr));
  e->type = type;

  switch (type) {
  case EXPR_TYPE_TERM:
    e->expr.term = (struct expr_term *)expr;
    break;
  case EXPR_TYPE_BINARY:
    e->expr.binary = (struct expr_binary *)expr;
    break;
  case EXPR_TYPE_FUNCCALL:
    e->expr.funccall = (struct expr_funccall *)expr;
    break;
  default:
    NOTIFY_ERRARGS(NOTIFY_ERR_UNKNOWN, "expr_alloc: unkown type: %d", type);
  }

  return e;
}

struct expr_funccall *
expr_funccall_alloc(struct token *id, struct vector(struct expr *) args)
{
  struct expr_funccall *e = utils_safe_malloc(sizeof(struct expr_funccall));
  e->id = id;
  e->args = args;
  return e;
}
