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
