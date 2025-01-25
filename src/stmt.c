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

struct stmt_let *
stmt_let_alloc(struct token *identifier, struct expr *expr, struct lexer *lexer) {
        (void)lexer;
        struct stmt_let *let
                = (struct stmt_let *)utils_s_malloc(sizeof(struct stmt_let), NULL, NULL);
        let->identifier = identifier;
        let->expr = expr;
        return let;
}

struct stmt_return*
stmt_return_alloc(struct expr *expr, struct lexer *lexer) {
        (void)lexer;
        struct stmt_return *ret
                = (struct stmt_return *)utils_s_malloc(sizeof(struct stmt_return), NULL, NULL);
        ret->expr = expr;
        return ret;
}

struct stmt_block *
stmt_block_alloc(struct stmt **stmts, size_t stmts_len, size_t stmts_cap, struct lexer *lexer) {
        (void)lexer;
        struct stmt_block *block
                = (struct stmt_block *)utils_s_malloc(sizeof(struct stmt_block), NULL, NULL);
        block->stmts = stmts;
        block->stmts_len = stmts_len;
        block->stmts_cap = stmts_cap;
        return block;
}

struct stmt_mut *
stmt_mut_alloc(struct expr *left, struct expr *right, struct token *op, struct lexer *lexer) {
        (void)lexer;
        struct stmt_mut *mut
                = (struct stmt_mut *)utils_s_malloc(sizeof(struct stmt_mut), NULL, NULL);
        mut->left = left;
        mut->right = right;
        mut->op = op;
        return mut;
}

struct stmt_expr *
stmt_expr_alloc(struct expr *expr, struct lexer *lexer) {
        (void)lexer;
        struct stmt_expr *stmt_expr
                = (struct stmt_expr *)utils_s_malloc(sizeof(struct stmt_expr), NULL, NULL);
        stmt_expr->expr = expr;
        return stmt_expr;
}

struct stmt_fn *
stmt_fn_alloc(struct token *id,
              struct token **params,
              size_t params_len,
              size_t params_cap,
              struct stmt_block *block,
              struct lexer *lexer) {
        (void)lexer;
        struct stmt_fn *fn
                = (struct stmt_fn *)utils_s_malloc(sizeof(struct stmt_fn), NULL, NULL);
        fn->id = id;
        fn->params = params;
        fn->params_len = params_len;
        fn->params_cap = params_cap;
        fn->block = block;
        return fn;
}

struct stmt *
stmt_alloc(void *data, enum stmt_type type, struct lexer *lexer) {
        (void)lexer;
        struct stmt *stmt
                = (struct stmt *)utils_s_malloc(sizeof(struct stmt), NULL, NULL);
        stmt->type = type;

        switch (type) {
        case STMT_TYPE_LET: {
                stmt->data.let = (struct stmt_let *)data;
        } break;
        case STMT_TYPE_BLOCK: {
                stmt->data.block = (struct stmt_block *)data;
        } break;
        case STMT_TYPE_MUT: {
                stmt->data.mut = (struct stmt_mut *)data;
        } break;
        case STMT_TYPE_EXPR: {
                stmt->data.expr = (struct stmt_expr *)data;
        } break;
        case STMT_TYPE_FN: {
                stmt->data.fn = (struct stmt_fn *)data;
        } break;
        case STMT_TYPE_RETURN: {
                stmt->data.ret = (struct stmt_return *)data;
        } break;
        default: {
                assert(0 && "statement unimplemented");
        } break;
        }

        return stmt;
}
