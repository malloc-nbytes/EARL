#include <stdlib.h>

#include "utils.h"
#include "ast.h"

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
stmt_mut_alloc()
{
  UNIMPLEMENTED("stmt_mut_alloc", NULL);
}

struct expr *
expr_alloc()
{
  UNIMPLEMENTED("expr_alloc", NULL);
}

