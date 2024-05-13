#include <stdlib.h>
#include "utils.h"
#include "ast.h"

struct stmt_def *
stmt_def_alloc(struct token *id)
{
  (void)id;
  DEBUG_ASSERT(0 && "unimplemented");
  return NULL;
}

struct stmt_let *
stmt_let_alloc()
{
  DEBUG_ASSERT(0 && "unimplemented");
  return NULL;
}

struct stmt_block *
stmt_block_alloc()
{
  DEBUG_ASSERT(0 && "unimplemented");
  return NULL;
}

struct stmt_mut *
stmt_mut_alloc()
{
  DEBUG_ASSERT(0 && "unimplemented");
  return NULL;
}

struct expr *
expr_alloc()
{
  DEBUG_ASSERT(0 && "unimplemented");
  return NULL;
}

