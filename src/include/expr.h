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
struct expr *expr_alloc(void *data, enum expr_type type, struct lexer *lexer);

/// @brief Allocate a binary expression
/// @param left The left expression
/// @param right The right expression
/// @param op The operator
/// @return The new expression
struct expr_binary *expr_binary_alloc(struct expr *left, struct expr *right, struct token *op, struct lexer *lexer);

/// @brief Allocate a unary expression
/// @param expr The right expression
/// @param op The operator
/// @return The new expression
struct expr_unary *expr_unary_alloc(struct expr *expr, struct token *op, struct lexer *lexer);

/// @brief Allocate an expression term
/// @param data The actual expression
/// @param type The type of the expression term
/// @return The new expression
struct expr_term *expr_term_alloc(void *data, enum expr_term_type term_type, struct lexer *lexer);

/// @brief Allocate an expression identifier
/// @param identifier The token of the identifier
/// @return The new expression
struct expr_identifier *expr_identifier_alloc(struct token *identifier, struct lexer *lexer);

/// @brief Allocate an expression integer literal
/// @param identifier The token of the integer
/// @return The new expression
struct expr_integer_literal *expr_integer_literal_alloc(struct token *integer, struct lexer *lexer);

/// @brief Allocate an expression string literal
/// @param identifier The token of the string
/// @return The new expression
struct expr_string_literal *expr_string_literal_alloc(struct token *string, struct lexer *lexer);

/// @brief Allocate an expression character literal
/// @param identifier The token of the character
/// @return The new expression
struct expr_character_literal *expr_character_literal_alloc(struct token *character, struct lexer *lexer);

/// @brief Allocate an expression function call
/// @param left The left side of the function call
/// @param args The argument list of expressions
/// @param len The length of the arguments
/// @param cap The capacity of the arguments
/// @return The new expression
struct expr_function_call *expr_function_call_alloc(struct expr *left,
                                                    struct expr **args,
                                                    size_t len,
                                                    size_t cap,
                                                    struct lexer *lexer);

#endif // EXPR_H
