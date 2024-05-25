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

// File: ast.h
// Description:
//   Provides a set of nodes for the abstract
//   syntax tree. This also defines the grammar.

#ifndef AST_H
#define AST_H

#include "token.hpp"

enum stmt_type {
  STMT_TYPE_DEF = 0,
  STMT_TYPE_LET,
  STMT_TYPE_BLOCK,
  STMT_TYPE_MUT,
  STMT_TYPE_STMT_EXPR,
};

enum expr_type {
  EXPR_TYPE_TERM = 0,
  EXPR_TYPE_BINARY,
  EXPR_TYPE_FUNCCALL,
};

enum expr_term_type {
  EXPR_TERM_TYPE_IDENT = 0,
  EXPR_TERM_TYPE_INTLIT,
  EXPR_TERM_TYPE_STRLIT,
};

struct stmt_def;
struct stmt_let;
struct stmt_block;
struct stmt_mut;
struct expr;

struct expr_binary;
struct expr_funccall;
struct expr_term;

struct stmt {
  enum stmt_type type;
  union {
    struct stmt_def *def;
    struct stmt_let *let;
    struct stmt_block *block;
    struct stmt_mut *mut;
    struct expr *expr;
  } stmt;
};

struct expr {
  enum expr_type type;
  union {
    struct expr_term *term;
    struct expr_binary *binary;
    struct expr_funccall *funccall;
  } expr;
};

struct expr_term {
  enum expr_term_type type;
  struct token *term;
};

struct expr_binary {
  struct expr *lhs;
  struct token *op;
  struct expr *rhs;
};

struct expr_funccall {
  struct token *id;
  /* struct vector(struct expr *) args; */
};

struct stmt_mut {
  struct expr *left;
  struct token *op;
  struct expr *right;
};

struct stmt_block {
  /* struct vector(struct stmt *) stmts; */
};

struct stmt_let {
  struct token *id;
  struct token *type;
  struct expr *expr;
};

struct stmt_def {
  struct token *id;
  /* struct vector(struct pair(struct token *id, struct token *type)) args; */
  struct token *rettype;
  struct stmt_block *block;
};

struct program {
  /* struct vector(struct stmt **) stmts; */
};

/*** STATEMENT CONSTRUCTORS ***/
struct stmt*     stmt_alloc(enum stmt_type type, void *stmt);
struct stmt_def* stmt_def_alloc(struct token *id,
                                /* struct vector(struct pair(struct token *id, struct token *type)) args, */
                                struct token *rettype,
                                struct stmt_block *block);

struct stmt_let* stmt_let_alloc(struct token *id,
                                struct token *type,
                                struct expr *expr);

/* struct stmt_block* stmt_block_alloc(struct vector(struct stmt *) stmts); */
struct stmt_mut*   stmt_mut_alloc(struct expr *lhs, struct token *op, struct expr *rhs);

/*** EXPRESSION CONSTRUCTORS ***/
struct expr*          expr_alloc(enum expr_type type, void *expr);
/* struct expr_funccall* expr_funccall_alloc(struct token *id, struct vector(struct expr *) args); */
struct expr_term*     expr_term_alloc(enum expr_term_type type, struct token *term);
struct expr_binary*   expr_binary_alloc(struct expr *lhs, struct token *op, struct expr *rhs);

/*** DEBUGGING ***/
void ast_dump(struct program *program);

#endif // AST_H
