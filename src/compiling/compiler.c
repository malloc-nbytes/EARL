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

#include "runtime/VM/opcode.h"
#include "runtime/builtins.h"
#include "runtime/EARL-value.h"
#include "runtime/EARL-object.h"
#include "runtime/primitives/string.h"

#include "ds/s-umap.h"

#include "mem/mem.h"

#include "misc/hash.h"
#include "misc/utils.h"
#include "misc/err.h"

static void cc_stmt(stmt_t *stmt, cc_t *cc);
static void cc_expr(expr_t *expr, cc_t *cc);

static int resolve_local(cc_t *cc, const char *id) {
    for (int i = (int)cc->locals.len - 1; i >= 0; --i) {
        local_t *local = &cc->locals.data[i];
        if (local->depth != -1 && local->depth < cc->scope_depth)
            return i;
        if (streq(id, local->id))
            err_wargs("identifier `%s` is already defined", id);
    }
    return -1;
}

void cc_add_local(const char *id, cc_t *cc) {
    if (cc->locals.len == LOCALS_LIM)
        err("too many local variables declared");
    local_t *local = &cc->locals.data[cc->locals.len++];
    local->id = id;
    local->depth = cc->scope_depth;
}

size_t cc_write_global(cc_t *cc, const char *id) {
    da_append(cc->gl_syms.data,
              cc->gl_syms.len,
              cc->gl_syms.cap,
              const char **,
              id);
    return cc->gl_syms.len-1;
}

size_t cc_write_to_const_pool(cc_t *cc, EARL_value_t value) {
    da_append(cc->constants.data,
              cc->constants.len,
              cc->constants.cap,
              EARL_value_t *,
              value);
    return cc->constants.len-1;
}

void cc_write_opcode(cc_t *cc, opcode_t opcode) {
    da_append(cc->opcode.data,
              cc->opcode.len,
              cc->opcode.cap,
              opcode_t *,
              opcode);
}

// Check if the identifier `id` is a global identifier.
// If it is, `idx` will be set to that index. The function
// will return `idx` if it is found, or NULL if not. This
// allows for easy conditionals.
static size_t *identifier_is_global(const cc_t *const cc, const char *id, size_t *idx) {
    *idx = SIZE_MAX;
    for (size_t i = 0; i < cc->gl_syms.len; ++i) {
        if (streq(id, cc->gl_syms.data[i])) {
            *idx = i;
            return idx;
        }
    }
    return NULL;
}

static void cc_expr_term_identifier(expr_identifier_t *expr, cc_t *cc) {
    const char *id = expr->identifier->lx;

    size_t idx;
    if (identifier_is_global(cc, id, &idx)) {
        cc_write_opcode(cc, OPCODE_LOAD_GLOBAL);
        cc_write_opcode(cc, idx);
    }
    else {
        ctx_assert_var_in_scope(cc->ctx, cc, id);

        cc_write_opcode(cc, OPCODE_LOAD_LOCAL);
        EARL_object_string_t *name = earl_object_string_alloc(id);
        size_t idx = cc_write_to_const_pool(cc, earl_value_object_create((EARL_object_t *)name));

        cc_write_opcode(cc, idx);
    }
    //err_wargs("identifier: `%s` is not defined", id);
}

static void cc_expr_term_integer_literal(expr_integer_literal_t *expr, cc_t *cc) {
    int integer = atoi(expr->integer->lx);
    size_t idx = cc_write_to_const_pool(cc, earl_value_integer_create(integer));
    cc_write_opcode(cc, OPCODE_CONST);
    cc_write_opcode(cc, idx);
}

static void cc_expr_term_string_literal(expr_string_literal_t *expr, cc_t *cc) {
    const char *s = expr->string->lx;
    EARL_object_string_t *str = earl_object_string_alloc(s);
    size_t idx = cc_write_to_const_pool(cc, earl_value_object_create((EARL_object_t *)str));
    cc_write_opcode(cc, OPCODE_CONST);
    cc_write_opcode(cc, idx);
}

static void cc_expr_term_function_call(expr_function_call_t *expr, cc_t *cc) {
    for (size_t i = 0; i < expr->args_len; ++i)
        cc_expr(expr->args[i], cc);

    cc_expr(expr->left, cc);
    cc_write_opcode(cc, OPCODE_CALL);
    cc_write_opcode(cc, (opcode_t)expr->args_len);
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
        cc_write_opcode(cc, OPCODE_ADD);
        break;
    case TOKEN_TYPE_MINUS:
        cc_write_opcode(cc, OPCODE_MINUS);
        break;
    case TOKEN_TYPE_FORWARD_SLASH:
        cc_write_opcode(cc, OPCODE_DIV);
        break;
    case TOKEN_TYPE_ASTERISK:
        cc_write_opcode(cc, OPCODE_MUL);
        break;
    case TOKEN_TYPE_PERCENT:
        cc_write_opcode(cc, OPCODE_MOD);
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
    // Compile the right-hand side expression first
    cc_expr(stmt->right, cc);

    // Get identifier for the left-hand side
    if (stmt->left->type != EXPR_TYPE_TERM || stmt->left->data.term->type != EXPR_TERM_TYPE_IDENTIFIER) {
        err("left-hand side of assignment must be an identifier");
    }

    const char *id = stmt->left->data.term->data.identifier->identifier->lx;

    // Find the variable in the global symbol table
    size_t idx;
    if (!identifier_is_global(cc, id, &idx))
        err_wargs("identifier `%s` is not defined", id);

    switch (stmt->op->type) {
    case TOKEN_TYPE_EQUALS:
        cc_write_opcode(cc, OPCODE_SET_GLOBAL);
        cc_write_opcode(cc, idx);
        break;

    case TOKEN_TYPE_PLUS_EQUALS:
    case TOKEN_TYPE_MINUS_EQUALS:
    case TOKEN_TYPE_ASTERISK_EQUALS:
    case TOKEN_TYPE_FORWARD_SLASH_EQUALS:
    case TOKEN_TYPE_PERCENT_EQUALS: {
        cc_write_opcode(cc, OPCODE_LOAD_GLOBAL);
        cc_write_opcode(cc, idx);

        // Apply the arithmetic operation
        switch (stmt->op->type) {
        case TOKEN_TYPE_PLUS_EQUALS:
            cc_write_opcode(cc, OPCODE_ADD);
            break;
        case TOKEN_TYPE_MINUS_EQUALS:
            cc_write_opcode(cc, OPCODE_MINUS);
            break;
        case TOKEN_TYPE_ASTERISK_EQUALS:
            cc_write_opcode(cc, OPCODE_MUL);
            break;
        case TOKEN_TYPE_FORWARD_SLASH_EQUALS:
            cc_write_opcode(cc, OPCODE_DIV);
            break;
        case TOKEN_TYPE_PERCENT_EQUALS:
            cc_write_opcode(cc, OPCODE_MOD);
            break;
        default:
            err_wargs("unknown compound assignment operator: %s", stmt->op->lx);
        }

        // Store back the result
        cc_write_opcode(cc, OPCODE_SET_GLOBAL);
        cc_write_opcode(cc, idx);
        break;
    }

    default:
        err_wargs("unknown assignment operator: %s", stmt->op->lx);
    }
}

static void cc_stmt_block(stmt_block_t *stmt, cc_t *cc) {
    ++cc->scope_depth;

    for (size_t i = 0; i < stmt->stmts_len; ++i)
        cc_stmt(stmt->stmts[i], cc);

    --cc->scope_depth;

    while (cc->locals.len > 0 && cc->locals.data[cc->locals.len-1].depth > cc->scope_depth) {
        cc_write_opcode(cc, OPCODE_POP);
        --cc->locals.len;
    }
}

static void cc_stmt_let(stmt_let_t *stmt, cc_t *cc) {
    const char *id = stmt->identifier->lx;
    //ctx_assert_var_not_in_scope(cc->ctx, cc, id);

    int arg = resolve_local(cc, id);

    // Local scope
    //if (cc->scope_depth == 0) {
    if (arg != -1) {
        cc_expr(stmt->expr, cc);
        cc_write_opcode(cc, OPCODE_SET_LOCAL);
        cc_write_opcode(cc, arg);
        //ctx_add_var_to_scope(cc->ctx, id);
    }
    // Global scope
    else {
        size_t idx = cc_write_global(cc, id);
        cc_expr(stmt->expr, cc);
        cc_write_opcode(cc, OPCODE_DEF_GLOBAL);
        cc_write_opcode(cc, idx);
    }
}

static void cc_stmt_fn(stmt_fn_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

static void cc_stmt(stmt_t *stmt, cc_t *cc) {
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
        .constants = {
            .data = mem_s_malloc(sizeof(EARL_value_t *) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .gl_syms = {
            .data = mem_s_malloc(sizeof(const char *) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .locals = { {0}, 0 },
        .scope_depth = 0,
    };

    __builtin_idents_init(&cc);

    for (size_t i = 0; i < prog->stmts_len; ++i)
        cc_stmt(prog->stmts[i], &cc);

    ctx_destroy(cc.ctx);

    cc_write_opcode(&cc, OPCODE_HALT);

    return cc;
}
