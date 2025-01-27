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
#include "expr.h"
#include "ast.h"

expr_function_call_t *expr_function_call_alloc(expr_t *left,
                                               expr_t **args,
                                               size_t len,
                                               size_t cap,
                                               lexer_t *lexer) {
    (void)lexer;

    expr_function_call_t *fn
        = (expr_function_call_t *)s_malloc(sizeof(expr_function_call_t), NULL, NULL);

    fn->left = left;
    fn->args = args;
    fn->args_len = len;
    fn->args_cap = cap;
    return fn;
}

expr_character_literal_t *expr_character_literal_alloc(token_t *character, lexer_t *lexer) {
    (void)lexer;
    expr_character_literal_t *chl
        = (expr_character_literal_t *)s_malloc(sizeof(expr_character_literal_t), NULL, NULL);
    chl->character = character;
    return chl;
}

expr_string_literal_t *expr_string_literal_alloc(token_t *string, lexer_t *lexer) {
    (void)lexer;
    expr_string_literal_t *strl
        = (expr_string_literal_t *)s_malloc(sizeof(expr_string_literal_t), NULL, NULL);
    strl->string = string;
    return strl;
}

expr_integer_literal_t *expr_integer_literal_alloc(token_t *integer, lexer_t *lexer) {
    (void)lexer;
    expr_integer_literal_t *il
        = (expr_integer_literal_t *)s_malloc(sizeof(expr_integer_literal_t), NULL, NULL);
    il->integer = integer;
    return il;
}

expr_identifier_t *expr_identifier_alloc(token_t *identifier, lexer_t *lexer) {
    (void)lexer;
    expr_identifier_t *il
        = (expr_identifier_t *)s_malloc(sizeof(expr_identifier_t), NULL, NULL);
    il->identifier = identifier;
    return il;
}

expr_unary_t *expr_unary_alloc(expr_t *expr, token_t *op, lexer_t *lexer) {
    (void)lexer;
    expr_unary_t *eu
        = (expr_unary_t *)s_malloc(sizeof(expr_unary_t), NULL, NULL);
    eu->expr = expr;
    eu->op = op;
    return eu;
}

expr_binary_t *expr_binary_alloc(expr_t *left, expr_t *right, token_t *op, lexer_t *lexer) {
    (void)lexer;
    expr_binary_t *bin
        = (expr_binary_t *)s_malloc(sizeof(expr_binary_t), NULL, NULL);
    bin->left = left;
    bin->right = right;
    bin->op = op;
    return bin;
}

expr_term_t *expr_term_alloc(void *data, expr_term_type_t type, lexer_t *lexer) {
    (void)lexer;
    expr_term_t *term = (expr_term_t *)s_malloc(sizeof(expr_term_t), NULL, NULL);
    term->type = type;

    switch (type) {
    case EXPR_TERM_TYPE_IDENTIFIER: {
        term->data.identifier = (expr_identifier_t *)data;
    } break;
    case EXPR_TERM_TYPE_INTEGER_LITERAL: {
        term->data.integer_literal = (expr_integer_literal_t *)data;
    } break;
    case EXPR_TERM_TYPE_STRING_LITERAL: {
        term->data.string_literal = (expr_string_literal_t *)data;
    } break;
    case EXPR_TERM_TYPE_FUNCTION_CALL: {
        term->data.function_call = (expr_function_call_t *)data;
    } break;
    case EXPR_TERM_TYPE_CHARACTER_LITERAL: {
        term->data.character_literal = (expr_character_literal_t *)data;
    } break;
    default: {
        assert(0 && "unimplemented");
    } break;
    }

    return term;
}

expr_t *expr_alloc(void *data, expr_type_t type, lexer_t *lexer) {
    (void)lexer;
    expr_t *expr = (expr_t *)s_malloc(sizeof(expr_t), NULL, NULL);
    expr->type = type;

    switch (type) {
    case EXPR_TYPE_TERM: {
        expr->data.term = (expr_term_t *)data;
    } break;
    case EXPR_TYPE_BINARY: {
        expr->data.binary = (expr_binary_t *)data;
    } break;
    case EXPR_TYPE_UNARY: {
        expr->data.unary = (expr_unary_t *)data;
    } break;
    default: {
        assert(0 && "unreachable");
    }
    }

    return expr;
}
