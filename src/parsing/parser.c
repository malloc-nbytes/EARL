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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "parsing/parser.h"
#include "parsing/expr.h"
#include "parsing/stmt.h"
#include "parsing/ast.h"
#include "lexing/lexer.h"
#include "lexing/token.h"
#include "global/attr.h"
#include "global/keywords.h"
#include "mem/mem.h"
#include "misc/utils.h"
#include "misc/err.h"

static stmt_t *parse_stmt(lexer_t *lexer);
static expr_t *parse_expr(lexer_t *lexer);

token_t *expect(lexer_t *lexer, token_type_t type) {
    token_t *tok = lexer_next(lexer);
    if (!tok || tok->type != type) {
        fprintf(stderr, "expected `%s` but got `%s`\n", token_type_to_cstr(type), token_type_to_cstr(tok->type));
        exit(1);
    }
    return tok;
}

token_t *expect_keyword(lexer_t *lexer, const char *kw) {
    token_t *tok = lexer_next(lexer);
    if (!tok || tok->type != TOKEN_TYPE_KEYWORD || !streq(tok->lx, kw)) {
        fprintf(stderr, "expected keyword `%s` but got `%s`\n", kw, tok->lx);
        exit(1);
    }
    return tok;
}

uint32_t translate_attr(lexer_t *lexer) {
    (void)expect(lexer, TOKEN_TYPE_AT);
    const char *s = lexer_peek(lexer, 0)->lx;

    if (streq(s, ATTR_STR_PUB))
        return (uint32_t)ATTR_PUB;
    if (streq(s, ATTR_STR_CONST))
        return (uint32_t)ATTR_CONST;
    if (streq(s, ATTR_STR_CONST))
        return (uint32_t)ATTR_REF;
    if (streq(s, ATTR_STR_EXPERIMENTAL))
        return (uint32_t)ATTR_EXPERIMENTAL;

    fprintf(stderr, "errror: unknown attribute: %s", s);
    exit(1);
}

static expr_t **parse_comma_sep_exprs(lexer_t *lexer, size_t *len, size_t *cap) {
    *len = 0, *cap = 1;
    expr_t **exprs = (expr_t **)mem_s_malloc(sizeof(expr_t *) * (*cap), NULL, NULL);

    while (1) {
        if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_RIGHT_PARENTHESIS)
            break;
        expr_t *expr = parse_expr(lexer);
        da_append(exprs, *len, *cap, expr_t **, expr);
        if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_COMMA)
            lexer_discard(lexer); // ,
        else
            break;
    }

    return exprs;
}

static token_t **parse_comma_sep_identifiers(lexer_t *lexer, size_t *len, size_t *cap) {
    *len = 0, *cap = 1;
    token_t **tokens = (token_t **)mem_s_malloc(sizeof(token_t *) * (*cap), NULL, NULL);

    while (1) {
        if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_RIGHT_PARENTHESIS)
            break;
        token_t *tok = expect(lexer, TOKEN_TYPE_IDENTIFIER);
        da_append(tokens, *len, *cap, token_t **, tok);
        if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_COMMA)
            lexer_discard(lexer); // ,
        else
            break;
    }

    return tokens;
}

static expr_t *parse_primary_expr(lexer_t *lexer) {
    expr_t *left = NULL;

    // Unary expressions
    while (lexer_ok(lexer) && (lexer_peek(lexer, 0)->type == TOKEN_TYPE_MINUS
                               || lexer_peek(lexer, 0)->type == TOKEN_TYPE_BANG
                               || lexer_peek(lexer, 0)->type == TOKEN_TYPE_BACKTICK_TILDE
                               || lexer_peek(lexer, 0)->type == TOKEN_TYPE_PLUS)) {
        token_t *op = lexer_next(lexer);
        expr_t *operand = parse_primary_expr(lexer);
        expr_unary_t *unary = expr_unary_alloc(operand, op, lexer);
        left = expr_alloc((void *)unary, EXPR_TYPE_UNARY, lexer);
    }

    while (1) {
        token_t *cur = lexer_peek(lexer, 0);
        if (!cur)
            return left;
        switch (cur->type) {
        case TOKEN_TYPE_IDENTIFIER: {
            expr_identifier_t *ident = expr_identifier_alloc(lexer_next(lexer), lexer);
            expr_term_t *term = expr_term_alloc((void *)ident, EXPR_TERM_TYPE_IDENTIFIER, lexer);
            left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
        } break;

        case TOKEN_TYPE_LEFT_PARENTHESIS: {
            lexer_discard(lexer); // (

            // function call
            if (left && left->type == EXPR_TYPE_TERM
                && left->data.term->type == EXPR_TERM_TYPE_IDENTIFIER) {
                size_t len, cap;
                expr_t **args = parse_comma_sep_exprs(lexer, &len, &cap);
                expr_function_call_t *fn = expr_function_call_alloc(left, args, len, cap, lexer);
                expr_term_t *term = expr_term_alloc((void *)fn, EXPR_TERM_TYPE_FUNCTION_CALL, lexer);
                left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
            }
            else
                left = parse_expr(lexer);
            (void)expect(lexer, TOKEN_TYPE_RIGHT_PARENTHESIS);
        } break;

        case TOKEN_TYPE_INTEGER_LITERAL: {
            expr_integer_literal_t *intlit = expr_integer_literal_alloc(lexer_next(lexer), lexer);
            expr_term_t *term = expr_term_alloc((void *)intlit, EXPR_TERM_TYPE_INTEGER_LITERAL, lexer);
            left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
        } break;

        case TOKEN_TYPE_STRING_LITERAL: {
            expr_string_literal_t *strlit = expr_string_literal_alloc(lexer_next(lexer), lexer);
            expr_term_t *term = expr_term_alloc((void *)strlit, EXPR_TERM_TYPE_STRING_LITERAL, lexer);
            left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
        } break;

        case TOKEN_TYPE_CHARACTER_LITERAL: {
            expr_character_literal_t *charlit = expr_character_literal_alloc(lexer_next(lexer), lexer);
            expr_term_t *term = expr_term_alloc((void *)charlit, EXPR_TERM_TYPE_CHARACTER_LITERAL, lexer);
            left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
        } break;

        case TOKEN_TYPE_KEYWORD: {
            if (streq(cur->lx, KEYWORD_IF))
                return left;
            err_wargs("keyword `%s` is not implemented in primary expression", cur->lx);
        } break;

        default: return left;
        }
    }

    assert(0 && "unreachable");
    return NULL; // unreachable
}

// static expr_t *
// parse_power_expr(struct lexer *lexer) {}

static expr_t *parse_multiplicitate_expr(lexer_t *lexer) {
    expr_t *lhs = parse_primary_expr(lexer);
    token_t *cur = lexer_peek(lexer, 0);
    while (cur && (cur->type == TOKEN_TYPE_ASTERISK
                   || cur->type == TOKEN_TYPE_FORWARD_SLASH
                   || cur->type == TOKEN_TYPE_PERCENT)) {
        token_t *op = lexer_next(lexer);
        expr_t *rhs = parse_primary_expr(lexer);
        expr_binary_t *bin = expr_binary_alloc(lhs, rhs, op, lexer);
        lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
        cur = lexer_peek(lexer, 0);
    }
    return lhs;
}

static expr_t *parse_additive_expr(lexer_t *lexer) {
    expr_t *lhs = parse_multiplicitate_expr(lexer);
    token_t *cur = lexer_peek(lexer, 0);
    while (cur && (cur->type == TOKEN_TYPE_PLUS
                   || cur->type == TOKEN_TYPE_MINUS)) {
        token_t *op = lexer_next(lexer);
        expr_t *rhs = parse_multiplicitate_expr(lexer);
        expr_binary_t *bin = expr_binary_alloc(lhs, rhs, op, lexer);
        lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
        cur = lexer_peek(lexer, 0);
    }
    return lhs;
}

static expr_t *parse_equalitative_expr(lexer_t *lexer) {
    expr_t *lhs = parse_additive_expr(lexer);
    token_t *cur = lexer_peek(lexer, 0);
    while (cur && (cur->type == TOKEN_TYPE_DOUBLE_EQUALS
                   || cur->type == TOKEN_TYPE_GREATERTHAN_EQUALS
                   || cur->type == TOKEN_TYPE_GREATERTHAN
                   || cur->type == TOKEN_TYPE_LESSTHAN_EQUALS
                   || cur->type == TOKEN_TYPE_LESSTHAN
                   || cur->type == TOKEN_TYPE_BANG_EQUALS)) {
        token_t *op = lexer_next(lexer);
        expr_t *rhs = parse_additive_expr(lexer);
        expr_binary_t *bin = expr_binary_alloc(lhs, rhs, op, lexer);
        lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
        cur = lexer_peek(lexer, 0);
    }
    return lhs;
}

static expr_t *parse_logical_expr(lexer_t *lexer) {
    expr_t *lhs = parse_equalitative_expr(lexer);
    token_t *cur = lexer_peek(lexer, 0);
    while (cur && (cur->type == TOKEN_TYPE_DOUBLE_AMPERSAND
                   || cur->type == TOKEN_TYPE_DOUBLE_PIPE)) {
        token_t *op = lexer_next(lexer);
        expr_t *rhs = parse_equalitative_expr(lexer);
        expr_binary_t *bin = expr_binary_alloc(lhs, rhs, op, lexer);
        lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
        cur = lexer_peek(lexer, 0);
    }
    return lhs;
}

static expr_t *parse_bitwise_expr(lexer_t *lexer) {
    (void)lexer;
    TODO;
}

static expr_t *parse_slice_expr(lexer_t *lexer) {
    (void)lexer;
    TODO;
}

static expr_t *parse_range_expr(lexer_t *lexer) {
    (void)lexer;
    TODO;
}

static expr_t *parse_expr(lexer_t *lexer) {
    return parse_logical_expr(lexer);
}

static stmt_block_t *parse_stmt_block(lexer_t *lexer) {
    size_t len = 0, cap = 1;
    stmt_t **stmts = (stmt_t **)mem_s_malloc(sizeof(stmt_t *) * cap, NULL, NULL);

    while (lexer_speek(lexer, 0)->type != TOKEN_TYPE_RIGHT_CURLY_BRACKET) {
        stmt_t *stmt = parse_stmt(lexer);
        da_append(stmts, len, cap, stmt_t **, stmt);
    }

    (void)expect(lexer, TOKEN_TYPE_RIGHT_CURLY_BRACKET);
    return stmt_block_alloc(stmts, len, cap, lexer);
}

static stmt_let_t *parse_stmt_let(lexer_t *lexer) {
    lexer_discard(lexer); // let
    token_t *ident = expect(lexer, TOKEN_TYPE_IDENTIFIER);
    (void)expect(lexer, TOKEN_TYPE_EQUALS);
    expr_t *expr = parse_expr(lexer);
    (void)expect(lexer, TOKEN_TYPE_SEMICOLON);
    return stmt_let_alloc(ident, expr, lexer);
}

static stmt_fn_t *parse_stmt_fn(lexer_t *lexer) {
    lexer_discard(lexer); // fn
    token_t *id = expect(lexer, TOKEN_TYPE_IDENTIFIER);
    (void)expect(lexer, TOKEN_TYPE_LEFT_PARENTHESIS);
    size_t len, cap;
    token_t **params = parse_comma_sep_identifiers(lexer, &len, &cap);
    (void)expect(lexer, TOKEN_TYPE_RIGHT_PARENTHESIS);
    (void)expect(lexer, TOKEN_TYPE_LEFT_CURLY_BRACKET);
    stmt_block_t *block = parse_stmt_block(lexer);
    (void)expect(lexer, TOKEN_TYPE_RIGHT_CURLY_BRACKET);
    return stmt_fn_alloc(id, params, len, cap, block, lexer);
}

static stmt_return_t *parse_stmt_return(lexer_t *lexer) {
    lexer_discard(lexer); // return
    expr_t *expr = parse_expr(lexer);
    (void)expect(lexer, TOKEN_TYPE_SEMICOLON);
    return stmt_return_alloc(expr, lexer);
}

static stmt_if_t *parse_stmt_if(lexer_t *lexer) {
    lexer_discard(lexer); // if
    expr_t *condition = parse_expr(lexer);

    (void)expect(lexer, TOKEN_TYPE_LEFT_CURLY_BRACKET);
    stmt_block_t *then_block = parse_stmt_block(lexer);

    // Handle `else if` or `else` blocks if applicable.
    stmt_block_t *else_block = NULL;
    token_t *tok1 = lexer_peek(lexer, 0);
    token_t *tok2 = lexer_peek(lexer, 1);

    int tok1_else = tok1 && tok1->type == TOKEN_TYPE_KEYWORD && streq(tok1->lx, KEYWORD_ELSE);
    int tok2_if   = tok2 && tok2->type == TOKEN_TYPE_KEYWORD && streq(tok2->lx, KEYWORD_IF);

    if (tok1_else && tok2_if) {
        stmt_t **tmp = (stmt_t **)mem_s_malloc(sizeof(stmt_t *), NULL, NULL);
        size_t
            tmp_len = 1,
            tmp_cap = 1;
        lexer_discard(lexer); // else
        stmt_if_t *nested_if = parse_stmt_if(lexer);
        stmt_t *tmp_stmt = stmt_alloc((void *)nested_if, STMT_TYPE_IF, lexer);
        tmp[0] = tmp_stmt;
        else_block = stmt_block_alloc(tmp, tmp_len, tmp_cap, lexer);
    } else if (tok1_else) {
        (void)expect_keyword(lexer, KEYWORD_ELSE);
        (void)expect(lexer, TOKEN_TYPE_LEFT_CURLY_BRACKET);
        else_block = parse_stmt_block(lexer);
    }

    return stmt_if_alloc(condition, then_block, else_block, lexer);
}

static stmt_t *parse_keyword_stmt(lexer_t *lexer) {
    const char *kw = lexer_peek(lexer, 0)->lx;
    if (streq(kw, KEYWORD_LET)) {
        stmt_let_t *let = parse_stmt_let(lexer);
        return stmt_alloc((void *)let, STMT_TYPE_LET, lexer);
    } else if (streq(kw, KEYWORD_FN)) {
        stmt_fn_t *fn = parse_stmt_fn(lexer);
        return stmt_alloc((void *)fn, STMT_TYPE_FN, lexer);
    } else if (streq(kw, KEYWORD_RET)) {
        stmt_return_t *ret = parse_stmt_return(lexer);
        return stmt_alloc((void *)ret, STMT_TYPE_RETURN, lexer);
    } else if (streq(kw, KEYWORD_IF)) {
        stmt_if_t *if_ = parse_stmt_if(lexer);
        return stmt_alloc((void *)if_, STMT_TYPE_IF, lexer);
    }
    fprintf(stderr, "unhandled keyword: %s\n", lexer_peek(lexer, 0)->lx);
    exit(1);
}

static stmt_t *parse_stmt_mut_or_stmt_expr(lexer_t *lexer) {
    expr_t *left = parse_expr(lexer);

    token_t *op = lexer_peek(lexer, 0);
    if (op && (op->type == TOKEN_TYPE_EQUALS ||
               op->type == TOKEN_TYPE_PLUS_EQUALS ||
               op->type == TOKEN_TYPE_MINUS_EQUALS ||
               op->type == TOKEN_TYPE_ASTERISK_EQUALS ||
               op->type == TOKEN_TYPE_FORWARD_SLASH_EQUALS)) {
        lexer_discard(lexer); // =, +=, -=, etc.
        expr_t *right = parse_expr(lexer);
        (void)expect(lexer, TOKEN_TYPE_SEMICOLON);
        stmt_mut_t *mut_stmt = (stmt_mut_t *)mem_s_malloc(sizeof(stmt_mut_t), NULL, NULL);
        mut_stmt->left = left;
        mut_stmt->right = right;
        mut_stmt->op = op;
        return stmt_alloc((void *)mut_stmt, STMT_TYPE_MUT, lexer);
    }

    (void)expect(lexer, TOKEN_TYPE_SEMICOLON);
    stmt_expr_t *stmt_expr = stmt_expr_alloc(left, lexer);
    return stmt_alloc((void *)stmt_expr, STMT_TYPE_EXPR, lexer);
}

static stmt_t *parse_stmt(lexer_t *lexer) {
    uint32_t attrs = 0x0;
    do {
        token_t *tok = lexer_peek(lexer, 0);
        assert(tok);

        switch (tok->type) {
        case TOKEN_TYPE_KEYWORD: {
            return parse_keyword_stmt(lexer);
        } break;
        case TOKEN_TYPE_LEFT_CURLY_BRACKET: {
            (void)expect(lexer, TOKEN_TYPE_LEFT_CURLY_BRACKET);
            stmt_block_t *block = parse_stmt_block(lexer);
            stmt_t *stmt = stmt_alloc((void *)block, STMT_TYPE_BLOCK, lexer);
            return stmt;
        } break;
        case TOKEN_TYPE_AT: {
            attrs |= translate_attr(lexer);
        } break;
        default: {
            return parse_stmt_mut_or_stmt_expr(lexer);
            /* stmt_expr_t *expr = parse_stmt_expr(lexer); */
            /* return stmt_alloc((void *)expr, STMT_TYPE_EXPR, lexer); */
        } break;
        }
    } while (attrs != 0x0);

    fprintf(stderr, "something went really wrong");
    exit(1);
}

program_t *parser_parse(lexer_t *lexer) {
    (void)parse_range_expr;
    (void)parse_slice_expr;
    (void)parse_bitwise_expr;

    program_t *prog = (program_t *)mem_s_malloc(sizeof(program_t), NULL, NULL);
    prog->stmts_len = 0;
    prog->stmts_cap = 32;
    prog->stmts = (stmt_t **)mem_s_malloc(sizeof(stmt_t *) * prog->stmts_cap, NULL, NULL);

    while (lexer_peek(lexer, 0) && lexer_peek(lexer, 0)->type != TOKEN_TYPE_EOF) {
        stmt_t *stmt = parse_stmt(lexer);
        da_append(prog->stmts,
                  prog->stmts_len,
                  prog->stmts_cap,
                  stmt_t **,
                  stmt);
    }

    return prog;
}
