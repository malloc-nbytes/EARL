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

#include "expr.h"
#include "stmt.h"
#include "attr.h"
#include "token.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "keywords.h"

static struct stmt *parse_stmt(struct lexer *lexer);
static struct expr *parse_expr(struct lexer *lexer);

struct token *
expect(struct lexer *lexer, enum token_type type) {
        struct token *tok = lexer_next(lexer);
        if (!tok || tok->type != type) {
                fprintf(stderr, "expected `%s` but got `%s`\n", token_type_to_cstr(type), token_type_to_cstr(tok->type));
                exit(1);
        }
        return tok;
}

uint32_t
translate_attr(struct lexer *lexer) {
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

static struct expr **
parse_comma_sep_exprs(struct lexer *lexer, size_t *len, size_t *cap) {
        *len = 0, *cap = 1;
        struct expr **exprs = (struct expr **)s_malloc(sizeof(struct expr *) * (*cap), NULL, NULL);

        while (1) {
                if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_RIGHT_PARENTHESIS)
                        break;
                struct expr *expr = parse_expr(lexer);
                da_append(exprs, *len, *cap, struct expr **, expr);
                if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_COMMA)
                        lexer_discard(lexer); // ,
                else
                        break;
        }

        return exprs;
}

static struct token **
parse_comma_sep_identifiers(struct lexer *lexer, size_t *len, size_t *cap) {
        *len = 0, *cap = 1;
        struct token **tokens = (struct token **)s_malloc(sizeof(struct token *) * (*cap), NULL, NULL);

        while (1) {
                if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_RIGHT_PARENTHESIS)
                        break;
                struct token *tok = expect(lexer, TOKEN_TYPE_IDENTIFIER);
                da_append(tokens, *len, *cap, struct token **, tok);
                if (lexer_speek(lexer, 0)->type == TOKEN_TYPE_COMMA)
                        lexer_discard(lexer); // ,
                else
                        break;
        }

        return tokens;
}

static struct expr *
parse_primary_expr(struct lexer *lexer) {
        struct expr *left = NULL;

        // Unary expressions
        while (lexer_ok(lexer) && (lexer_peek(lexer, 0)->type == TOKEN_TYPE_MINUS
                                   || lexer_peek(lexer, 0)->type == TOKEN_TYPE_BANG
                                   || lexer_peek(lexer, 0)->type == TOKEN_TYPE_BACKTICK_TILDE
                                   || lexer_peek(lexer, 0)->type == TOKEN_TYPE_PLUS)) {
                struct token *op = lexer_next(lexer);
                struct expr *operand = parse_primary_expr(lexer);
                struct expr_unary *unary = expr_unary_alloc(operand, op, lexer);
                left = expr_alloc((void *)unary, EXPR_TYPE_UNARY, lexer);
        }

        while (1) {
                struct token *cur = lexer_peek(lexer, 0);
                if (!cur)
                        return left;
                switch (cur->type) {
                        case TOKEN_TYPE_IDENTIFIER: {
                                struct expr_identifier *ident = expr_identifier_alloc(lexer_next(lexer), lexer);
                                struct expr_term *term = expr_term_alloc((void *)ident, EXPR_TERM_TYPE_IDENTIFIER, lexer);
                                left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
                        } break;

                        case TOKEN_TYPE_LEFT_PARENTHESIS: {
                                lexer_discard(lexer); // (

                                // function call
                                if (left && left->type == EXPR_TYPE_TERM
                                        && left->data.term->type == EXPR_TERM_TYPE_IDENTIFIER) {
                                        size_t len, cap;
                                        struct expr **args = parse_comma_sep_exprs(lexer, &len, &cap);
                                        struct expr_function_call *fn = expr_function_call_alloc(left, args, len, cap, lexer);
                                        struct expr_term *term = expr_term_alloc((void *)fn, EXPR_TERM_TYPE_FUNCTION_CALL, lexer);
                                        left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
                                }
                                else
                                        left = parse_expr(lexer);
                                (void)expect(lexer, TOKEN_TYPE_RIGHT_PARENTHESIS);
                        } break;

                        case TOKEN_TYPE_INTEGER_LITERAL: {
                                struct expr_integer_literal *intlit = expr_integer_literal_alloc(lexer_next(lexer), lexer);
                                struct expr_term *term = expr_term_alloc((void *)intlit, EXPR_TERM_TYPE_INTEGER_LITERAL, lexer);
                                left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
                        } break;

                        case TOKEN_TYPE_STRING_LITERAL: {
                                struct expr_string_literal *strlit = expr_string_literal_alloc(lexer_next(lexer), lexer);
                                struct expr_term *term = expr_term_alloc((void *)strlit, EXPR_TERM_TYPE_STRING_LITERAL, lexer);
                                left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
                        } break;

                        case TOKEN_TYPE_CHARACTER_LITERAL: {
                                struct expr_character_literal *charlit = expr_character_literal_alloc(lexer_next(lexer), lexer);
                                struct expr_term *term = expr_term_alloc((void *)charlit, EXPR_TERM_TYPE_CHARACTER_LITERAL, lexer);
                                left = expr_alloc((void *)term, EXPR_TYPE_TERM, lexer);
                        } break;

                        case TOKEN_TYPE_KEYWORD: {
                                assert(0);
                        } break;

                        default: return left;
                }
        }

        assert(0 && "unreachable");
        return NULL; // unreachable
}

// static struct expr *
// parse_power_expr(struct lexer *lexer) {}

static struct expr *
parse_multiplicitate_expr(struct lexer *lexer) {
        struct expr *lhs = parse_primary_expr(lexer);
        struct token *cur = lexer_peek(lexer, 0);
        while (cur && (cur->type == TOKEN_TYPE_ASTERISK
                || cur->type == TOKEN_TYPE_FORWARD_SLASH
                || cur->type == TOKEN_TYPE_PERCENT)) {
                struct token *op = lexer_next(lexer);
                struct expr *rhs = parse_primary_expr(lexer);
                struct expr_binary *bin = expr_binary_alloc(lhs, rhs, op, lexer);
                lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
                cur = lexer_peek(lexer, 0);
        }
        return lhs;
}

static struct expr *
parse_additive_expr(struct lexer *lexer) {
        struct expr *lhs = parse_multiplicitate_expr(lexer);
        struct token *cur = lexer_peek(lexer, 0);
        while (cur && (cur->type == TOKEN_TYPE_PLUS
                || cur->type == TOKEN_TYPE_MINUS)) {
                struct token *op = lexer_next(lexer);
                struct expr *rhs = parse_multiplicitate_expr(lexer);
                struct expr_binary *bin = expr_binary_alloc(lhs, rhs, op, lexer);
                lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
                cur = lexer_peek(lexer, 0);
        }
        return lhs;
}

static struct expr *
parse_equalitative_expr(struct lexer *lexer) {
        struct expr *lhs = parse_additive_expr(lexer);
        struct token *cur = lexer_peek(lexer, 0);
        while (cur && (cur->type == TOKEN_TYPE_DOUBLE_EQUALS
                || cur->type == TOKEN_TYPE_GREATERTHAN_EQUALS
                || cur->type == TOKEN_TYPE_GREATERTHAN
                || cur->type == TOKEN_TYPE_LESSTHAN_EQUALS
                || cur->type == TOKEN_TYPE_LESSTHAN
                || cur->type == TOKEN_TYPE_BANG_EQUALS)) {
                struct token *op = lexer_next(lexer);
                struct expr *rhs = parse_additive_expr(lexer);
                struct expr_binary *bin = expr_binary_alloc(lhs, rhs, op, lexer);
                lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
                cur = lexer_peek(lexer, 0);
        }
        return lhs;
}

static struct expr *
parse_logical_expr(struct lexer *lexer) {
        struct expr *lhs = parse_equalitative_expr(lexer);
        struct token *cur = lexer_peek(lexer, 0);
        while (cur && (cur->type == TOKEN_TYPE_DOUBLE_AMPERSAND
                || cur->type == TOKEN_TYPE_DOUBLE_PIPE)) {
                struct token *op = lexer_next(lexer);
                struct expr *rhs = parse_equalitative_expr(lexer);
                struct expr_binary *bin = expr_binary_alloc(lhs, rhs, op, lexer);
                lhs = expr_alloc((void *)bin, EXPR_TYPE_BINARY, lexer);
                cur = lexer_peek(lexer, 0);
        }
        return lhs;
}

// static struct expr *
// parse_bitwise_expr(struct lexer *lexer) {}

// static struct expr *
// parse_slice_expr(struct lexer *lexer) {}

// static struct expr *
// parse_range_expr(struct lexer *lexer) {}

static struct expr *
parse_expr(struct lexer *lexer) {
        return parse_logical_expr(lexer);
}

static struct stmt_block *
parse_stmt_block(struct lexer *lexer) {
        size_t len = 0, cap = 1;
        struct stmt **stmts = (struct stmt **)s_malloc(sizeof(struct stmt *) * cap, NULL, NULL);

        while (lexer_speek(lexer, 0)->type != TOKEN_TYPE_RIGHT_CURLY_BRACKET) {
                struct stmt *stmt = parse_stmt(lexer);
                da_append(stmts, len, cap, struct stmt **, stmt);
        }

        return stmt_block_alloc(stmts, len, cap, lexer);
}

static struct stmt_let *
parse_stmt_let(struct lexer *lexer) {
        lexer_discard(lexer); // let
        struct token *ident = expect(lexer, TOKEN_TYPE_IDENTIFIER);
        (void)expect(lexer, TOKEN_TYPE_EQUALS);
        struct expr *expr = parse_expr(lexer);
        (void)expect(lexer, TOKEN_TYPE_SEMICOLON);
        return stmt_let_alloc(ident, expr, lexer);
}

static struct stmt_fn *
parse_stmt_fn(struct lexer *lexer) {
        lexer_discard(lexer); // fn
        struct token *id = expect(lexer, TOKEN_TYPE_IDENTIFIER);
        (void)expect(lexer, TOKEN_TYPE_LEFT_PARENTHESIS);
        size_t len, cap;
        struct token **params = parse_comma_sep_identifiers(lexer, &len, &cap);
        (void)expect(lexer, TOKEN_TYPE_RIGHT_PARENTHESIS);
        (void)expect(lexer, TOKEN_TYPE_LEFT_CURLY_BRACKET);
        struct stmt_block *block = parse_stmt_block(lexer);
        (void)expect(lexer, TOKEN_TYPE_RIGHT_CURLY_BRACKET);
        return stmt_fn_alloc(id, params, len, cap, block, lexer);
}

static struct stmt_return *
parse_stmt_return(struct lexer *lexer) {
        lexer_discard(lexer); // return
        struct expr *expr = parse_expr(lexer);
        (void)expect(lexer, TOKEN_TYPE_SEMICOLON);
        return stmt_return_alloc(expr, lexer);
}

static struct stmt *
parse_keyword_stmt(struct lexer *lexer) {
        const char *kw = lexer_peek(lexer, 0)->lx;
        if (streq(kw, KEYWORD_LET)) {
                struct stmt_let *let = parse_stmt_let(lexer);
                return stmt_alloc((void *)let, STMT_TYPE_LET, lexer);
        } else if (streq(kw, KEYWORD_FN)) {
                struct stmt_fn *fn = parse_stmt_fn(lexer);
                return stmt_alloc((void *)fn, STMT_TYPE_FN, lexer);
        } else if (streq(kw, KEYWORD_RET)) {
                struct stmt_return *ret = parse_stmt_return(lexer);
                return stmt_alloc((void *)ret, STMT_TYPE_RETURN, lexer);
        }
        fprintf(stderr, "unhandled keyword: %s\n", lexer_peek(lexer, 0)->lx);
        exit(1);
}

static struct stmt_expr *
parse_stmt_expr(struct lexer *lexer) {
        struct expr *expr = parse_expr(lexer);
        struct stmt_expr *stmt_expr = stmt_expr_alloc(expr, lexer);
        (void)expect(lexer, TOKEN_TYPE_SEMICOLON);
        return stmt_expr;
}

static struct stmt *
parse_stmt(struct lexer *lexer) {
        uint32_t attrs = 0x0;
        do {
                struct token *tok = lexer_peek(lexer, 0);
                assert(tok);

                switch (tok->type) {
                case TOKEN_TYPE_KEYWORD: {
                        return parse_keyword_stmt(lexer);
                } break;
                /* case TOKEN_TYPE_INTEGER_LITERAL: */
                /* case TOKEN_TYPE_IDENTIFIER: { */
                /*         struct stmt_expr *expr = parse_stmt_expr(lexer); */
                /*         return stmt_alloc((void *)expr, STMT_TYPE_EXPR, lexer); */
                /* } break; */
                case TOKEN_TYPE_AT: {
                        attrs |= translate_attr(lexer);
                } break;
                default: {
                        struct stmt_expr *expr = parse_stmt_expr(lexer);
                        return stmt_alloc((void *)expr, STMT_TYPE_EXPR, lexer);
                } break;
                /* default: { */
                /*         fprintf(stderr, "unhandled token type in parse_stmt: %s\n", token_type_to_cstr(tok->type)); */
                /*         exit(1); */
                /* } break; */
                }
        } while (attrs != 0x0);

        fprintf(stderr, "something went really wrong");
        exit(1);
}

struct program *
parser_parse(struct lexer *lexer) {
        struct program *prog = (struct program *)s_malloc(sizeof(struct program), NULL, NULL);
        prog->stmts_len = 0;
        prog->stmts_cap = 32;
        prog->stmts = (struct stmt **)s_malloc(sizeof(struct stmt *) * prog->stmts_cap, NULL, NULL);

        while (lexer_peek(lexer, 0) && lexer_peek(lexer, 0)->type != TOKEN_TYPE_EOF) {
                struct stmt *stmt = parse_stmt(lexer);
                da_append(prog->stmts,
                                prog->stmts_len,
                                prog->stmts_cap,
                                struct stmt **,
                                stmt);
        }

        return prog;
}
