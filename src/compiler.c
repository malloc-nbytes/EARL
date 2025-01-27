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
#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "ast.h"
#include "EARL-value.h"
#include "opcode.h"
#include "utils.h"
#include "err.h"
#include "s-umap.h"
#include "hash.h"
#include "ctx.h"

void cc_expr(expr_t *expr, cc_t *cc);

void push_opcode(cc_t *cc, opcode_t opc, size_t *const_pool_idx) {
    da_append(cc->opcode.data, cc->opcode.len, cc->opcode.cap, opcode_t *,
              opc);
    if (const_pool_idx)
        da_append(cc->opcode.data, cc->opcode.len, cc->opcode.cap,
                  opcode_t *, *const_pool_idx);
}

void cc_expr_term_identifier(expr_identifier_t *expr, cc_t *cc) {
    const char *id = expr->identifier->lx;
    ctx_assert_var_in_scope(cc->ctx, id);
    push_opcode(cc, OPCODE_LOAD, NULL);
}

void cc_expr_term_integer_literal(expr_integer_literal_t *expr, cc_t *cc) {
    int integer = atoi(expr->integer->lx);
    size_t idx = cc_push_constant(cc, EARL_VALUE_TYPE_INTEGER, &integer);
    push_opcode(cc, OPCODE_CONST, &idx);
}

void cc_expr_term_string_literal(expr_string_literal_t *expr, cc_t *cc) {
    (void)expr;
    (void)cc;
    TODO;
}

void cc_expr_term_function_call(expr_function_call_t *expr, cc_t *cc) {
    (void)expr;
    (void)cc;
    TODO;
}

void cc_expr_term(expr_term_t *expr, cc_t *cc) {
    switch (expr->type) {
    case EXPR_TERM_TYPE_IDENTIFIER:
        cc_expr_term_identifier(expr->data.identifier, cc);
        break;
    case EXPR_TERM_TYPE_INTEGER_LITERAL:
        cc_expr_term_integer_literal(expr->data.integer_literal, cc);
        break;
    case EXPR_TERM_TYPE_STRING_LITERAL:
        cc_expr_term_string_literal(expr->data.string_literal, cc);
        break;
    case EXPR_TERM_TYPE_FUNCTION_CALL:
        cc_expr_term_function_call(expr->data.function_call, cc);
        break;
    default: err_wargs("unknown expression term type: %d", (int)expr->type);
    }
}

void cc_expr_binary(expr_binary_t *expr, cc_t *cc) {
    cc_expr(expr->left, cc);
    cc_expr(expr->right, cc);
    switch (expr->op->type) {
    case TOKEN_TYPE_PLUS:
        push_opcode(cc, OPCODE_ADD, NULL);
        break;
    case TOKEN_TYPE_MINUS:
        push_opcode(cc, OPCODE_MINUS, NULL);
        break;
    case TOKEN_TYPE_FORWARD_SLASH:
        push_opcode(cc, OPCODE_DIV, NULL);
        break;
    case TOKEN_TYPE_ASTERISK:
        push_opcode(cc, OPCODE_MUL, NULL);
        break;
    case TOKEN_TYPE_PERCENT:
        push_opcode(cc, OPCODE_MOD, NULL);
        break;
    default:
        err_wargs("unknown binary operator: %s", expr->op->lx);
    }
}

void cc_expr_unary(expr_unary_t *expr, cc_t *cc) {
    (void)expr;
    (void)cc;
    TODO;
}

void cc_expr(expr_t *expr, cc_t *cc) {
    switch (expr->type) {
    case EXPR_TYPE_TERM:
        cc_expr_term(expr->data.term, cc);
        break;
    case EXPR_TYPE_BINARY:
        cc_expr_binary(expr->data.binary, cc);
        break;
    case EXPR_TYPE_UNARY:
        cc_expr_unary(expr->data.unary, cc);
        break;
    default: err_wargs("unknown expression type: %d", (int)expr->type);
    }
}

void cc_stmt_return(stmt_return_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

void cc_stmt_expr(stmt_expr_t *stmt, cc_t *cc) {
    cc_expr(stmt->expr, cc);
}

void cc_stmt_mut(stmt_mut_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

void cc_stmt_block(stmt_block_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

void cc_stmt_let(stmt_let_t *stmt, cc_t *cc) {
    const char *id = stmt->identifier->lx;
    ctx_assert_var_not_in_scope(cc->ctx, id);
    cc_expr(stmt->expr, cc);

    ctx_add_var_to_scope(cc->ctx, id);

    da_append(cc->gl_syms.data, cc->gl_syms.len, cc->gl_syms.cap, const char **, id);

    size_t idx = cc_push_constant(cc, EARL_VALUE_TYPE_INTEGER, (void *)&(int){cc->gl_syms.len-1});
    push_opcode(cc, OPCODE_STORE, &idx);
}

void
cc_stmt_fn(stmt_fn_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

void compile_stmt(stmt_t *stmt, cc_t *cc) {
    switch (stmt->type) {
    case STMT_TYPE_FN: cc_stmt_fn(stmt->data.fn, cc);                break;
    case STMT_TYPE_LET: cc_stmt_let(stmt->data.let, cc);             break;
    case STMT_TYPE_BLOCK: cc_stmt_block(stmt->data.block, cc);       break;
    case STMT_TYPE_MUT: cc_stmt_mut(stmt->data.mut, cc);             break;
    case STMT_TYPE_EXPR: cc_stmt_expr(stmt->data.expr, cc);          break;
    case STMT_TYPE_RETURN: cc_stmt_return(stmt->data.ret, cc);       break;
    default: {
        err_wargs("unknown statement type: %s",
                  stmt_type_to_cstr(stmt->type));
    } break;
    }
}

size_t cc_push_constant(cc_t *cc, EARL_value_type_t type, void *data) {
    EARL_value_t *v = EARL_value_alloc(type, data);
    da_append(cc->const_pool.data, cc->const_pool.len, cc->const_pool.cap, EARL_value_t **, v);
    return cc->const_pool.len-1;
}

void cc_dump_opcode(const cc_t cc) {
    printf("Dumping compiled opcode\n");
    for (size_t i = 0; i < cc.opcode.len; ++i)
        opcode_dump(cc.opcode.data[i]);
}

cc_t cc_compile(program_t *prog) {
    ctx_t ctx = ctx_create(djb2, djb2, NULL);

    const size_t CAP = 32;

    cc_t cc = (cc_t) {
        .ctx = &ctx,
        .opcode = {
            .data = s_malloc(sizeof(opcode_t) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .const_pool = {
            .data = s_malloc(sizeof(EARL_value_t *) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .gl_syms = {
            .data = s_malloc(sizeof(const char *) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
    };

    for (size_t i = 0; i < prog->stmts_len; ++i)
        compile_stmt(prog->stmts[i], &cc);

    ctx_destroy(cc.ctx);

    push_opcode(&cc, OPCODE_HALT, NULL);

    return cc;
}
