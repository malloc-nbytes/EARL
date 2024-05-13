#ifndef AST_H
#define AST_H

#include "token.h"
#include "vector.h"

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
  struct expr lhs;
  struct token *op;
  struct expr rhs;
};

struct expr_funccall {
  struct token *id;
  struct vector(struct expr *) args;
};

struct stmt_mut {
  struct expr left;
  struct token *op;
  struct expr right;
};

struct stmt_block {
  struct vector(struct stmt *) stmts;
};

struct stmt_let {
  struct token *id;
  struct token *type;
  struct expr *expr;
};

struct stmt_def {
  struct token *id;
  struct token *args;
  struct token *arg_types;
  struct token *rettype;
  struct stmt_block *block;
};

struct program {
  struct vector(struct stmt *) stmts;
};

/* struct stmt_def *stmt_def_alloc(struct token *id); */
/* struct stmt_let *stmt_let_alloc(); */
/* struct stmt_block *stmt_block_alloc(); */
/* struct stmt_mut *stmt_mut_alloc(); */
/* struct expr *expr_alloc(); */


#endif // AST_H
