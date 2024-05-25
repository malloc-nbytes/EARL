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
  struct stmt *s = static_cast<struct stmt *>(utils_safe_malloc(sizeof(struct stmt)));
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
    assert(0 && "fixme");
     // NOTIFY_ERRARGS(NOTIFY_ERR_UNKNOWN, "stmt_alloc: unkown type: %d", type);
  }

  return s;
}

struct stmt_def *
stmt_def_alloc(struct token *id,
               // struct vector(struct pair(struct token *id, struct token *type)) args,
               struct token *rettype,
               struct stmt_block *block)
{
  struct stmt_def *s = static_cast<struct stmt_def *>(utils_safe_malloc(sizeof(struct stmt_def)));
  s->id = id;
  // s->args = args;
  s->rettype = rettype;
  s->block = block;
  return s;
}

struct stmt_let *
stmt_let_alloc(struct token *id, struct token *type, struct expr *expr)
{
  struct stmt_let *s = static_cast<struct stmt_let *>(utils_safe_malloc(sizeof(struct stmt_let)));
  s->id = id;
  s->type = type;
  s->expr = expr;
  return s;
}

// struct stmt_block *
// stmt_block_alloc(struct vector(struct stmt *) stmts)
// {
//   struct stmt_block *s = utils_safe_malloc(sizeof(struct stmt_block));
//   s->stmts = stmts;
//   return s;
// }

struct stmt_mut *
stmt_mut_alloc(struct expr *lhs, struct token *op, struct expr *rhs)
{
  struct stmt_mut *s = static_cast<struct stmt_mut *>(utils_safe_malloc(sizeof(struct stmt_mut)));
  s->left = lhs;
  s->op = op;
  s->right = rhs;
  return s;
}

struct expr_term *
expr_term_alloc(enum expr_term_type type, struct token *term)
{
  struct expr_term *e = static_cast<struct expr_term *>(utils_safe_malloc(sizeof(struct expr_term)));
  e->type = type;
  e->term = term;
  return e;
}

struct expr_binary *
expr_binary_alloc(struct expr *lhs, struct token *op, struct expr *rhs)
{
  struct expr_binary *e = static_cast<struct expr_binary *>(utils_safe_malloc(sizeof(struct expr_binary)));
  e->lhs = lhs;
  e->op = op;
  e->rhs = rhs;
  return e;
}

struct expr *
expr_alloc(enum expr_type type, void *expr)
{
  struct expr *e = static_cast<struct expr *>(utils_safe_malloc(sizeof(struct expr)));
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
    assert(false && "fixme");
    // NOTIFY_ERRARGS(NOTIFY_ERR_UNKNOWN, "expr_alloc: unkown type: %d", type);
  }

  return e;
}

// struct expr_funccall *
// expr_funccall_alloc(struct token *id, struct vector(struct expr *) args)
// {
//   struct expr_funccall *e = utils_safe_malloc(sizeof(struct expr_funccall));
//   e->id = id;
//   e->args = args;
//   return e;
// }

/*** DEBUG DUMPING ***/

#define SPACES(n) for (int __i = 0; __i < n; ++__i) putchar(' ');

static void dump_expr(struct expr *expr, int spaces);

static void
dump_expr_term(struct expr_term *expr, int spaces)
{
  switch (expr->type) {
  case EXPR_TERM_TYPE_IDENT:
    SPACES(spaces); printf("(ident) %s\n", (expr->term)->lexeme);
    break;
  case EXPR_TERM_TYPE_INTLIT:
    SPACES(spaces); printf("(intlit) %s\n", (expr->term)->lexeme);
    break;
  case EXPR_TERM_TYPE_STRLIT:
    SPACES(spaces); printf("(strlit) %s\n", (expr->term)->lexeme);
    break;
  default:
    assert(false && "fixme");
    // NOTIFY_ERRARGS(NOTIFY_ERR_UNKNOWN, "dump_expr_term: unkown type: %d", expr->type);
  }
}

static void
dump_expr_funccall(struct expr_funccall *expr, int spaces)
{
  UNIMPLEMENTED("dump_expr_funccall");
}

static void
dump_expr_binary(struct expr_binary *expr, int spaces)
{
  SPACES(spaces); printf("lhs: ");
  dump_expr(expr->lhs, spaces);
  SPACES(spaces); printf("%s\n", (expr->op)->lexeme);
  SPACES(spaces); printf("rhs: ");
  dump_expr(expr->rhs, spaces);
}

static void
dump_expr(struct expr *expr, int spaces)
{
  switch (expr->type) {
  case EXPR_TYPE_BINARY:
    dump_expr_binary(expr->expr.binary, spaces+2);
    break;
  case EXPR_TYPE_FUNCCALL:
    dump_expr_funccall(expr->expr.funccall, spaces+2);
    break;
  case EXPR_TYPE_TERM:
    dump_expr_term(expr->expr.term, spaces+2);
    break;
  default:
    printf("unkown expr: %d\n", expr->type);
  }
}

static void
dump_stmt_def(struct stmt_def *stmt, int spaces)
{
  UNIMPLEMENTED("ast_dump_stmt_def");
}

static void
dump_stmt_let(struct stmt_let *stmt, int spaces)
{
  printf("LET %s %s =\n", stmt->type->lexeme, stmt->id->lexeme);
  dump_expr(stmt->expr, spaces+2);
}

static void
dump_stmt(struct stmt *stmt, int spaces)
{
  switch (stmt->type) {
  case STMT_TYPE_LET:
    dump_stmt_let(stmt->stmt.let, spaces);
    break;
  case STMT_TYPE_DEF:
    assert(0 && "STMT_TYPE_DEF unimplemented");
    break;
  case STMT_TYPE_BLOCK:
    assert(0 && "STMT_TYPE_BLOCK unimplemented");
    break;
  case STMT_TYPE_MUT:
    assert(0 && "STMT_TYPE_MUT unimplemented");
    break;
  case STMT_TYPE_STMT_EXPR:
    assert(0 && "STMT_TYPE_STMT_EXPR unimplemented");
    break;
  default:
    assert(false && "fixme");
     // NOTIFY_ERRARGS(NOTIFY_ERR_UNKNOWN, "ast_dump_stmt: unkown type: %d", stmt->type);
  }
}

void
ast_dump(struct program *program)
{
  // for (size_t i = 0; i < program->stmts.len; ++i) {
  //   struct vector(stmt *) *stmts = &program->stmts;
  //   struct stmt *stmt = *(struct stmt **)vector_at(stmts, i);
  //   dump_stmt(stmt, 0);
  // }
}

#undef SPACES