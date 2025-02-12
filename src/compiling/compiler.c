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

void cc_write_opcode(cc_t *cc, uint8_t opcode) {
    da_append(cc->opcode.data,
              cc->opcode.len,
              cc->opcode.cap,
              uint8_t *,
              opcode);
}

static int write_jump(cc_t *cc, uint8_t op) {
    cc_write_opcode(cc, op);
    cc_write_opcode(cc, (uint8_t)0xFF);
    cc_write_opcode(cc, (uint8_t)0xFF);
    return cc->opcode.len - 2;
}

static void patch_jump(cc_t *cc, int offset) {
    int jump = cc->opcode.len - offset - 2;

    if (jump > UINT16_MAX)
        err("too many statements in `if` statement");

    cc->opcode.data[offset] = (uint8_t)((jump >> 8) & 0xFF);
    cc->opcode.data[offset+1] = (uint8_t)(jump & 0xFF);
}

static int resolve_local(cc_t *cc, const char *id) {
    for (int i = cc->locals.len - 1; i >= 0; --i) {
        local_t *local = &cc->locals.data[i];
        if (streq(local->id, id))
            return i;
    }
    return -1;
}

static void add_local(cc_t *cc, const char *id) {
    if (cc->locals.len >= LOCALS_LIM)
        err("too many local variables declared");

    local_t *local = &cc->locals.data[cc->locals.len++];
    local->id = id;
    local->depth = cc->scope_depth;
}

uint8_t cc_write_to_const_pool(cc_t *cc, EARL_value_t value) {
    da_append(cc->constants.data,
              cc->constants.len,
              cc->constants.cap,
              EARL_value_t *,
              value);
    return (uint8_t)cc->constants.len-1;
}

uint8_t cc_write_str_to_const(cc_t *cc, const char *id) {
    EARL_object_string_t *name = earl_object_string_alloc(id);
    return cc_write_to_const_pool(cc, earl_value_object_create((EARL_object_t *)name));
}

static void cc_expr_term_identifier(expr_identifier_t *expr, cc_t *cc) {
    const char *id = expr->identifier->lx;

    // Determine if the variable is global or local.
    size_t arg = resolve_local(cc, id);

    // Opcode changes based on local or global.
    uint8_t op = arg == -1lu ? OPCODE_LOAD_GLOBAL : OPCODE_LOAD_LOCAL;

    // Write the lexeme to the const pool as an EARL string.
    size_t idx = cc_write_str_to_const(cc, id);

    // Write the appropriate opcode.
    cc_write_opcode(cc, op);

    // If global, we want the index in the constant pool.
    // If local, we want the stack position (arg).
    cc_write_opcode(cc, arg == -1lu ? idx : (uint8_t)arg);
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
    cc_write_opcode(cc, (uint8_t)expr->args_len);
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

    token_type_t opty = expr->op->type;

    // Wait to compile right side of the expression
    // for short-circuiting.
    if ((opty != TOKEN_TYPE_DOUBLE_AMPERSAND)
            && (opty != TOKEN_TYPE_DOUBLE_PIPE))
        cc_expr(expr->right, cc);

    switch (opty) {
    case TOKEN_TYPE_PLUS:
        cc_write_opcode(cc, OPCODE_ADD);
        break;
    case TOKEN_TYPE_MINUS:
        cc_write_opcode(cc, OPCODE_SUB);
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
    case TOKEN_TYPE_DOUBLE_AMPERSAND: {
        int end_jump = write_jump(cc, OPCODE_JUMP_IF_FALSE);
        cc_expr(expr->right, cc);
        patch_jump(cc, end_jump);
    } break;
    case TOKEN_TYPE_DOUBLE_PIPE: {
        int end_jump = write_jump(cc, OPCODE_JUMP_IF_TRUE);
        cc_expr(expr->right, cc);
        patch_jump(cc, end_jump);
    } break;
    case TOKEN_TYPE_DOUBLE_EQUALS:
        cc_write_opcode(cc, OPCODE_EQ);
        break;
    case TOKEN_TYPE_BANG_EQUALS:
        cc_write_opcode(cc, OPCODE_NEQ);
        break;
    case TOKEN_TYPE_LESSTHAN:
        cc_write_opcode(cc, OPCODE_LT);
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
    cc_write_opcode(cc, OPCODE_POP);
}

static void cc_stmt_mut(stmt_mut_t *stmt, cc_t *cc) {
    cc_expr(stmt->right, cc);

    // Get identifier for the left-hand side
    if (stmt->left->type != EXPR_TYPE_TERM || stmt->left->data.term->type != EXPR_TERM_TYPE_IDENTIFIER) {
        err("left-hand side of assignment must be an identifier");
    }

    const char *id = stmt->left->data.term->data.identifier->identifier->lx;

    // Check if it's a local or global variable
    int local_index = resolve_local(cc, id);
    int is_local = local_index != -1;

    size_t idx;
    if (!is_local) {
        // Global: write to constant pool
        EARL_object_string_t *name = earl_object_string_alloc(id);
        idx = cc_write_to_const_pool(cc, earl_value_object_create((EARL_object_t *)name));
    }

    switch (stmt->op->type) {
    case TOKEN_TYPE_EQUALS:
        cc_write_opcode(cc, is_local ? OPCODE_SET_LOCAL : OPCODE_SET_GLOBAL);
        cc_write_opcode(cc, is_local ? (uint8_t)local_index : (uint8_t)idx);
        break;

    case TOKEN_TYPE_PLUS_EQUALS:
    case TOKEN_TYPE_MINUS_EQUALS:
    case TOKEN_TYPE_ASTERISK_EQUALS:
    case TOKEN_TYPE_FORWARD_SLASH_EQUALS:
    case TOKEN_TYPE_PERCENT_EQUALS: {
        // Load current variable value
        cc_write_opcode(cc, is_local ? OPCODE_LOAD_LOCAL : OPCODE_LOAD_GLOBAL);
        cc_write_opcode(cc, is_local ? (uint8_t)local_index : (uint8_t)idx);

        // Apply the arithmetic operation
        switch (stmt->op->type) {
        case TOKEN_TYPE_PLUS_EQUALS:
            cc_write_opcode(cc, OPCODE_ADD);
            break;
        case TOKEN_TYPE_MINUS_EQUALS:
            cc_write_opcode(cc, OPCODE_SUB);
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
        cc_write_opcode(cc, is_local ? OPCODE_SET_LOCAL : OPCODE_SET_GLOBAL);
        cc_write_opcode(cc, is_local ? (uint8_t)local_index : (uint8_t)idx);
        break;
    }

    default:
        err_wargs("unknown assignment operator: %s", stmt->op->lx);
    }
}

static void begin_scope(cc_t *cc) {
   ++cc->scope_depth;
}

static void end_scope(cc_t *cc) {
    --cc->scope_depth;

    while (cc->locals.len > 0
           && cc->locals.data[cc->locals.len-1].depth > (int)cc->scope_depth) {
        cc_write_opcode(cc, OPCODE_POP);
        --cc->locals.len;
    }
}

static void cc_stmt_block(stmt_block_t *stmt, cc_t *cc) {
    begin_scope(cc);

    for (size_t i = 0; i < stmt->stmts_len; ++i)
        cc_stmt(stmt->stmts[i], cc);

    end_scope(cc);
}

static void declare_global_variable(cc_t *cc, const char *id) {
    EARL_object_string_t *name = earl_object_string_alloc(id);
    size_t idx = cc_write_to_const_pool(cc, earl_value_object_create((EARL_object_t *)name));
    cc_write_opcode(cc, OPCODE_DEF_GLOBAL);
    cc_write_opcode(cc, (uint8_t)idx);
}

static void declare_local_variable(cc_t *cc, const char *id) {
    for (int i = (int)cc->locals.len-1; i >= 0; --i) {
        local_t *local = &cc->locals.data[i];
        if (local->depth != -1 && local->depth < (int)cc->scope_depth)
            break;
        if (streq(id, local->id))
            err_wargs("identifier `%s` has already been defined", id);
    }
    add_local(cc, id);
}

static void cc_stmt_let(stmt_let_t *stmt, cc_t *cc) {
    const char *id = stmt->identifier->lx;

    cc_expr(stmt->expr, cc);

    if (!cc->scope_depth)
        declare_global_variable(cc, id);
    else
        declare_local_variable(cc, id);
}

static void cc_stmt_if(stmt_if_t *stmt, cc_t *cc) {
    cc_expr(stmt->condition, cc);

    int then_jump = write_jump(cc, OPCODE_JUMP_IF_FALSE);
    cc_write_opcode(cc, OPCODE_POP);

    cc_stmt_block(stmt->then_block, cc);

    int else_jump = -1;

    if (stmt->else_block) {
        else_jump = write_jump(cc, OPCODE_JUMP);
        patch_jump(cc, then_jump);
        cc_write_opcode(cc, OPCODE_POP);

        cc_stmt_block(stmt->else_block, cc);
    }
    else
        patch_jump(cc, then_jump);

    if (else_jump != -1)
        patch_jump(cc, else_jump);
}

static void cc_stmt_fn(stmt_fn_t *stmt, cc_t *cc) {
    (void)stmt;
    (void)cc;
    TODO;
}

static void write_loop(cc_t *cc, size_t loop_start) {
    cc_write_opcode(cc, OPCODE_LOOP);
    size_t offset = cc->opcode.len - loop_start + 2;
    if (offset > UINT16_MAX)
        err("too many statements in loop body");
    cc_write_opcode(cc, (uint8_t)((offset >> 8) & 0xFF));
    cc_write_opcode(cc, (uint8_t)(offset & 0xFF));
}

static void cc_stmt_while(stmt_while_t *stmt, cc_t *cc) {
    size_t loop_start = cc->opcode.len;
    cc_expr(stmt->condition, cc);

    int exit_jump = write_jump(cc, OPCODE_JUMP_IF_FALSE);
    cc_write_opcode(cc, OPCODE_POP);
    cc_stmt_block(stmt->block, cc);
    write_loop(cc, loop_start);

    patch_jump(cc, exit_jump);
    cc_write_opcode(cc, OPCODE_POP);
}

static void cc_stmt_for(stmt_for_t *stmt, cc_t *cc) {
    cc_expr(stmt->start, cc);

    // Declare the loop variable
    const char *id = stmt->enumerator->lx;
    declare_local_variable(cc, id);

    // Evaluate the start expression and assign it to the loop variable
    int local_index = resolve_local(cc, id);

    // Start of loop condition check
    size_t loop_start = cc->opcode.len;

    cc_write_opcode(cc, OPCODE_LOAD_LOCAL);
    cc_write_opcode(cc, (uint8_t)local_index);
    cc_expr(stmt->end, cc);
    cc_write_opcode(cc, OPCODE_LT);

    int exit_jump = write_jump(cc, OPCODE_JUMP_IF_FALSE);
    cc_write_opcode(cc, OPCODE_POP);

    // Execute loop body
    cc_stmt_block(stmt->block, cc);

    // Increment loop variable
    cc_write_opcode(cc, OPCODE_LOAD_LOCAL);
    cc_write_opcode(cc, (uint8_t)local_index);
    cc_write_opcode(cc, OPCODE_CONST);
    cc_write_opcode(cc, cc_write_to_const_pool(cc, earl_value_integer_create(1)));
    cc_write_opcode(cc, OPCODE_ADD);
    cc_write_opcode(cc, OPCODE_SET_LOCAL);
    cc_write_opcode(cc, (uint8_t)local_index);

    // Jump back to condition check
    write_loop(cc, loop_start);

    // Exit loop
    patch_jump(cc, exit_jump);
    cc_write_opcode(cc, OPCODE_POP);
}

static void cc_stmt(stmt_t *stmt, cc_t *cc) {
    switch (stmt->type) {
    case STMT_TYPE_FN:     cc_stmt_fn(stmt->data.fn, cc);        break;
    case STMT_TYPE_LET:    cc_stmt_let(stmt->data.let, cc);      break;
    case STMT_TYPE_BLOCK:  cc_stmt_block(stmt->data.block, cc);  break;
    case STMT_TYPE_MUT:    cc_stmt_mut(stmt->data.mut, cc);      break;
    case STMT_TYPE_EXPR:   cc_stmt_expr(stmt->data.expr, cc);    break;
    case STMT_TYPE_RETURN: cc_stmt_return(stmt->data.ret, cc);   break;
    case STMT_TYPE_IF:     cc_stmt_if(stmt->data.if_, cc);       break;
    case STMT_TYPE_WHILE:  cc_stmt_while(stmt->data.while_, cc); break;
    case STMT_TYPE_FOR:    cc_stmt_for(stmt->data.for_, cc);     break;
    default: {
        err_wargs("unknown statement type: %s",
                  stmt_type_to_cstr(stmt->type));
    } break;
    }
}

cc_t cc_compile(program_t *prog) {
    const size_t CAP = 32;

    cc_t cc = (cc_t) {
        .opcode = {
            .data = (uint8_t *)mem_s_malloc(sizeof(uint8_t) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .constants = {
            .data = (EARL_value_t *)mem_s_malloc(sizeof(EARL_value_t) * CAP, NULL, NULL),
            .len = 0,
            .cap = CAP,
        },
        .locals = {
            .data = {{0}},
            .len = 0,
        },
        .scope_depth = 0,
    };

    __builtin_idents_init(&cc);

    for (size_t i = 0; i < prog->stmts_len; ++i)
        cc_stmt(prog->stmts[i], &cc);

    cc_write_opcode(&cc, OPCODE_HALT);

    return cc;
}
