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

#include "compiling/compiler.h"
#include "compiling/ctx.h"

#include "parsing/ast.h"

#include "runtime/builtins.h"
#include "runtime/EARL-value.h"
#include "runtime/VM/opcode.h"

#include "ds/s-umap.h"

#include "mem/mem.h"

#include "misc/hash.h"
#include "misc/utils.h"
#include "misc/err.h"

static void cc_expr(expr_t *expr, cc_t *cc);

size_t cc_push_constant(cc_t *cc, EARL_value_type_t type, void *data) {
    EARL_value_t *v = EARL_value_alloc(type, data);
    da_append(cc->const_pool.data, cc->const_pool.len, cc->const_pool.cap, EARL_value_t **, v);
    return cc->const_pool.len-1;
}

size_t cc_push_global(cc_t *cc, const char *id) {
    da_append(cc->gl_syms.data, cc->gl_syms.len, cc->gl_syms.cap, const char **, id);
    return cc->gl_syms.len-1;
}

static void push_opcode(cc_t *cc, opcode_t opc) {
    da_append(cc->opcode.data, cc->opcode.len, cc->opcode.cap, opcode_t *, opc);
}

static void push_index_from_const_or_gl_pool(cc_t *cc, size_t idx) {
    da_append(cc->opcode.data, cc->opcode.len, cc->opcode.cap, opcode_t *, idx);
}

static void cc_expr_term_identifier(expr_identifier_t *expr, cc_t *cc) {
    const char *id = expr->identifier->lx;

    ctx_assert_var_in_scope(cc->ctx, cc, id);

    size_t idx = SIZE_MAX;
    for (size_t i = 0; i < cc->gl_syms.len; ++i) {
        if (strcmp(cc->gl_syms.data[i], id) == 0) {
            idx = i; // Found the symbol
            break;
        }
    }

    if (idx == SIZE_MAX) {
        assert("unreachable");
        // idx = cc_push_global(cc, id);
    }

    push_opcode(cc, OPCODE_LOAD);
    push_index_from_const_or_gl_pool(cc, idx);
}

static void cc_expr_term_integer_literal(expr_integer_literal_t *expr, cc_t *cc) {
    int integer = atoi(expr->integer->lx);
    size_t idx = cc_push_constant(cc, EARL_VALUE_TYPE_INTEGER, &integer);

    push_opcode(cc, OPCODE_CONST);
    push_index_from_const_or_gl_pool(cc, idx);
}

static void cc_expr_term_string_literal(expr_string_literal_t *expr, cc_t *cc) {
    (void)expr;
    (void)cc;
    TODO;
}

static void cc_expr_term_function_call(expr_function_call_t *expr, cc_t *cc) {
    for (size_t i = 0; i < expr->args_len; ++i)
        cc_expr(expr->args[i], cc);

    cc_expr(expr->left, cc);
    push_opcode(cc, OPCODE_CALL);
    push_opcode(cc, (opcode_t)expr->args_len);
}

static void cc_expr_term(expr_term_t *expr, cc_t *cc) {
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

static void cc_expr_binary(expr_binary_t *expr, cc_t *cc) {
    cc_expr(expr->left, cc);
    cc_expr(expr->right, cc);
    switch (expr->op->type) {
    case TOKEN_TYPE_PLUS:
        push_opcode(cc, OPCODE_ADD);
        break;
    case TOKEN_TYPE_MINUS:
        push_opcode(cc, OPCODE_MINUS);
        break;
    case TOKEN_TYPE_FORWARD_SLASH:
        push_opcode(cc, OPCODE_DIV);
        break;
    case TOKEN_TYPE_ASTERISK:
        push_opcode(cc, OPCODE_MUL);
        break;
    case TOKEN_TYPE_PERCENT:
        push_opcode(cc, OPCODE_MOD);
        break;
    default:
        err_wargs("unknown binary operator: %s", expr->op->lx);
    }
}

static void cc_expr_unary(expr_unary_t *expr, cc_t *cc) {
    (void)expr;
    (void)cc;
    TODO;
}

static void cc_expr(expr_t *expr, cc_t *cc) {
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

static void cc_stmt_return(stmt_return_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

static void cc_stmt_expr(stmt_expr_t *stmt, cc_t *cc) {
    cc_expr(stmt->expr, cc);
}

static void cc_stmt_mut(stmt_mut_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

static void cc_stmt_block(stmt_block_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

static void cc_stmt_let(stmt_let_t *stmt, cc_t *cc) {
    const char *id = stmt->identifier->lx;
    ctx_assert_var_not_in_scope(cc->ctx, cc, id);

    size_t var_idx = cc_push_global(cc, id);

    cc_expr(stmt->expr, cc);
    ctx_add_var_to_scope(cc->ctx, id);

    push_opcode(cc, OPCODE_STORE);
    push_opcode(cc, var_idx);
}

static void cc_stmt_fn(stmt_fn_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

static void compile_stmt(stmt_t *stmt, cc_t *cc) {
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

void cc_dump_gl_syms(const cc_t *cc) {
    for (size_t i = 0; i < cc->gl_syms.len; ++i)
        printf("GL_SYM: %s\n", cc->gl_syms.data[i]);
}

void cc_dump_opcode(const cc_t cc) {
    printf("Dumping compiled opcode\n");
    for (size_t i = 0; i < cc.opcode.len; ++i)
        opcode_dump(cc.opcode.data[i]);
}

cc_t cc_compile(program_t *prog) {
    ctx_t ctx = ctx_create(djb2);

    const size_t CAP = 32;

    cc_t cc = (cc_t) {
        .ctx = &ctx,
        .opcode = {
            .data = mem_s_malloc(sizeof(opcode_t) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .const_pool = {
            .data = mem_s_malloc(sizeof(EARL_value_t *) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .gl_syms = {
            .data = mem_s_malloc(sizeof(const char *) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
    };

    __builtin_idents_init(&cc);

    for (size_t i = 0; i < prog->stmts_len; ++i)
        compile_stmt(prog->stmts[i], &cc);

    ctx_destroy(cc.ctx);

    push_opcode(&cc, OPCODE_HALT);

    return cc;
}
