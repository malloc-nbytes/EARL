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

#ifndef EXPR_H
#define EXPR_H

#include "lexer.h"
#include "token.h"
#include "ast.h"

/// @brief Allocate an expression
/// @param data The actual expression
/// @param type The type of the expression
/// @return The new expression
expr_t *expr_alloc(void *data, expr_type_t type, lexer_t *lexer);

/// @brief Allocate a binary expression
/// @param left The left expression
/// @param right The right expression
/// @param op The operator
/// @return The new expression
expr_binary_t *expr_binary_alloc(expr_t *left, expr_t *right, token_t *op, lexer_t *lexer);

/// @brief Allocate a unary expression
/// @param expr The right expression
/// @param op The operator
/// @return The new expression
expr_unary_t *expr_unary_alloc(expr_t *expr, token_t *op, lexer_t *lexer);

/// @brief Allocate an expression term
/// @param data The actual expression
/// @param type The type of the expression term
/// @return The new expression
expr_term_t *expr_term_alloc(void *data, expr_term_type_t term_type, lexer_t *lexer);

/// @brief Allocate an expression identifier
/// @param identifier The token of the identifier
/// @return The new expression
expr_identifier_t *expr_identifier_alloc(token_t *identifier, lexer_t *lexer);

/// @brief Allocate an expression integer literal
/// @param identifier The token of the integer
/// @return The new expression
expr_integer_literal_t *expr_integer_literal_alloc(token_t *integer, lexer_t *lexer);

/// @brief Allocate an expression string literal
/// @param identifier The token of the string
/// @return The new expression
expr_string_literal_t *expr_string_literal_alloc(token_t *string, lexer_t *lexer);

/// @brief Allocate an expression function call
/// @param left The left side of the function call
/// @param args The argument list of expressions
/// @param len The length of the arguments
/// @param cap The capacity of the arguments
/// @return The new expression
expr_function_call_t *expr_function_call_alloc(expr_t *left,
                                               expr_t **args,
                                               size_t len,
                                               size_t cap,
                                               lexer_t *lexer);

/// @brief Allocate an expression character literal
/// @param identifier The token of the character
/// @return The new expression
expr_character_literal_t *expr_character_literal_alloc(token_t *character, lexer_t *lexer);

#endif // EXPR_H
