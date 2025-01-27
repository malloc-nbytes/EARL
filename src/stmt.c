/** file */

// MIT License

// Copyright (c) 2025 malloc-nbytes

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

#include <assert.h>

#include "utils.h"
#include "stmt.h"

stmt_let_t *stmt_let_alloc(token_t *identifier, expr_t *expr, lexer_t *lexer) {
    (void)lexer;
    stmt_let_t *let = (stmt_let_t *)s_malloc(sizeof(stmt_let_t), NULL, NULL);
    let->identifier = identifier;
    let->expr = expr;
    return let;
}

stmt_return_t *stmt_return_alloc(expr_t *expr, lexer_t *lexer) {
    (void)lexer;
    stmt_return_t *ret = (stmt_return_t *)s_malloc(sizeof(stmt_return_t), NULL, NULL);
    ret->expr = expr;
    return ret;
}

stmt_block_t *stmt_block_alloc(stmt_t **stmts, size_t stmts_len, size_t stmts_cap, lexer_t *lexer) {
    (void)lexer;
    stmt_block_t *block = (stmt_block_t *)s_malloc(sizeof(stmt_block_t), NULL, NULL);
    block->stmts = stmts;
    block->stmts_len = stmts_len;
    block->stmts_cap = stmts_cap;
    return block;
}

stmt_mut_t *stmt_mut_alloc(expr_t *left, expr_t *right, token_t *op, lexer_t *lexer) {
    (void)lexer;
    stmt_mut_t *mut = (stmt_mut_t *)s_malloc(sizeof(stmt_mut_t), NULL, NULL);
    mut->left = left;
    mut->right = right;
    mut->op = op;
    return mut;
}

stmt_expr_t *stmt_expr_alloc(expr_t *expr, lexer_t *lexer) {
    (void)lexer;
    stmt_expr_t *stmt_expr = (stmt_expr_t *)s_malloc(sizeof(stmt_expr_t), NULL, NULL);
    stmt_expr->expr = expr;
    return stmt_expr;
}

stmt_fn_t *stmt_fn_alloc(token_t *id,
                         token_t **params,
                         size_t params_len,
                         size_t params_cap,
                         stmt_block_t *block,
                         lexer_t *lexer) {
    (void)lexer;
    stmt_fn_t *fn
        = (stmt_fn_t *)s_malloc(sizeof(stmt_fn_t), NULL, NULL);
    fn->id = id;
    fn->params = params;
    fn->params_len = params_len;
    fn->params_cap = params_cap;
    fn->block = block;
    return fn;
}

stmt_t *stmt_alloc(void *data, stmt_type_t type, lexer_t *lexer) {
    (void)lexer;
    stmt_t *stmt
        = (stmt_t *)s_malloc(sizeof(stmt_t), NULL, NULL);
    stmt->type = type;

    switch (type) {
    case STMT_TYPE_LET: {
        stmt->data.let = (stmt_let_t *)data;
    } break;
    case STMT_TYPE_BLOCK: {
        stmt->data.block = (stmt_block_t *)data;
    } break;
    case STMT_TYPE_MUT: {
        stmt->data.mut = (stmt_mut_t *)data;
    } break;
    case STMT_TYPE_EXPR: {
        stmt->data.expr = (stmt_expr_t *)data;
    } break;
    case STMT_TYPE_FN: {
        stmt->data.fn = (stmt_fn_t *)data;
    } break;
    case STMT_TYPE_RETURN: {
        stmt->data.ret = (stmt_return_t *)data;
    } break;
    default: {
        assert(0 && "statement unimplemented");
    } break;
    }

    return stmt;
}
