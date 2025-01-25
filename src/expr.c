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

struct expr_function_call *
expr_function_call_alloc(struct expr *left,
                         struct expr **args,
                         size_t len,
                         size_t cap,
                         struct lexer *lexer) {
        (void)lexer;

        struct expr_function_call *fn
                = (struct expr_function_call *)utils_s_malloc(sizeof(struct expr_function_call), NULL, NULL);
        fn->left = left;
        fn->args = args;
        fn->args_len = len;
        fn->args_cap = cap;
        return fn;
}

struct expr_character_literal *
expr_character_literal_alloc(struct token *character, struct lexer *lexer) {
        (void)lexer;
        struct expr_character_literal *chl
                = (struct expr_character_literal *)utils_s_malloc(sizeof(struct expr_character_literal), NULL, NULL);
        chl->character = character;
        return chl;
}

struct expr_string_literal *
expr_string_literal_alloc(struct token *string, struct lexer *lexer) {
        (void)lexer;
        struct expr_string_literal *strl
                = (struct expr_string_literal *)utils_s_malloc(sizeof(struct expr_string_literal), NULL, NULL);
        strl->string = string;
        return strl;
}

struct expr_integer_literal *
expr_integer_literal_alloc(struct token *integer, struct lexer *lexer) {
        (void)lexer;
        struct expr_integer_literal *il
                = (struct expr_integer_literal *)utils_s_malloc(sizeof(struct expr_integer_literal), NULL, NULL);
        il->integer = integer;
        return il;
}

struct expr_identifier *
expr_identifier_alloc(struct token *identifier, struct lexer *lexer) {
        (void)lexer;
        struct expr_identifier *il
                = (struct expr_identifier *)utils_s_malloc(sizeof(struct expr_identifier), NULL, NULL);
        il->identifier = identifier;
        return il;
}

struct expr_unary *
expr_unary_alloc(struct expr *expr, struct token *op, struct lexer *lexer) {
        (void)lexer;
        struct expr_unary *eu
                = (struct expr_unary *)utils_s_malloc(sizeof(struct expr_unary), NULL, NULL);
        eu->expr = expr;
        eu->op = op;
        return eu;
}

struct expr_binary *
expr_binary_alloc(struct expr *left, struct expr *right, struct token *op, struct lexer *lexer) {
        (void)lexer;
        struct expr_binary *bin
                = (struct expr_binary *)utils_s_malloc(sizeof(struct expr_binary), NULL, NULL);
        bin->left = left;
        bin->right = right;
        bin->op = op;
        return bin;
}

struct expr_term *
expr_term_alloc(void *data, enum expr_term_type type, struct lexer *lexer) {
        (void)lexer;
        struct expr_term *term = (struct expr_term *)utils_s_malloc(sizeof(struct expr_term), NULL, NULL);
        term->type = type;

        switch (type) {
        case EXPR_TERM_TYPE_IDENTIFIER: {
                term->data.identifier = (struct expr_identifier *)data;
        } break;
        case EXPR_TERM_TYPE_INTEGER_LITERAL: {
                term->data.integer_literal = (struct expr_integer_literal *)data;
        } break;
        case EXPR_TERM_TYPE_STRING_LITERAL: {
                term->data.string_literal = (struct expr_string_literal *)data;
        } break;
        case EXPR_TERM_TYPE_FUNCTION_CALL: {
                term->data.function_call = (struct expr_function_call *)data;
        } break;
        case EXPR_TERM_TYPE_CHARACTER_LITERAL: {
                term->data.character_literal = (struct expr_character_literal *)data;
        } break;
        default: {
                assert(0 && "unimplemented");
        } break;
        }

        return term;
}

struct expr *
expr_alloc(void *data, enum expr_type type, struct lexer *lexer) {
        (void)lexer;
        struct expr *expr = (struct expr *)utils_s_malloc(sizeof(struct expr), NULL, NULL);
        expr->type = type;

        switch (type) {
                case EXPR_TYPE_TERM: {
                        expr->data.term = (struct expr_term *)data;
                } break;
                case EXPR_TYPE_BINARY: {
                        expr->data.binary = (struct expr_binary *)data;
                } break;
                case EXPR_TYPE_UNARY: {
                        expr->data.unary = (struct expr_unary *)data;
                } break;
                default: {
                        assert(0 && "unreachable");
                }
        }

        return expr;
}
