#include <stdlib.h>

#include "utils.h"
#include "ast.h"

struct stmt *
stmt_alloc(enum stmt_type type, void *stmt)
{
  NOOP(type);
  NOOP(stmt);
  UNIMPLEMENTED("stmt_alloc", NULL);
}

struct stmt_def *
stmt_def_alloc(struct token *id,
               struct pair(struct token *id, struct token *type) args,
               struct token *rettype,
               struct stmt_block *block)
{
  NOOP(id);
  NOOP(args);
  NOOP(rettype);
  NOOP(block);
  UNIMPLEMENTED("stmt_def_alloc", NULL);
}

struct stmt_let *
stmt_let_alloc(struct token *id, struct token *type, struct token *expr)
{
  NOOP(id);
  NOOP(type);
  NOOP(expr);
  UNIMPLEMENTED("stmt_let_alloc", NULL);
}

struct stmt_block *
stmt_block_alloc(struct vector(struct stmt *) stmts)
{
  NOOP(stmts);
  UNIMPLEMENTED("stmt_block_alloc", NULL);
}

struct stmt_mut *
stmt_mut_alloc(struct expr *lhs, struct token *op, struct expr *rhs)
{
  NOOP(lhs);
  NOOP(op);
  NOOP(rhs);
  UNIMPLEMENTED("stmt_mut_alloc", NULL);
}

struct expr_term *
expr_term_alloc(enum expr_term_type type, struct token *term)
{
  NOOP(type);
  NOOP(term);
  UNIMPLEMENTED("expr_term_alloc", NULL);
}

struct expr_binary *
expr_binary_alloc(struct expr lhs, struct token *op, struct expr rhs)
{
  NOOP(lhs);
  NOOP(op);
  NOOP(rhs);
  UNIMPLEMENTED("expr_binary_alloc", NULL);
}

struct expr *
expr_alloc(enum expr_type type, void *expr)
{
  NOOP(type);
  NOOP(expr);
  UNIMPLEMENTED("expr_alloc", NULL);
}

struct expr_funccall *
expr_funccall_alloc(struct token *id, struct vector(struct expr *) args)
{
  NOOP(id);
  NOOP(args);
  UNIMPLEMENTED("expr_funccall_alloc", NULL);
}
