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

#ifndef STMT_H
#define STMT_H

#include "lexer.h"
#include "ast.h"

/// @brief Allocate a 'let' statement
/// @param identifier The identifier
/// @param expr The expression
/// @param lexer The lexer to use
/// @return The new statement
struct stmt_let *stmt_let_alloc(struct token *identifier, struct expr *expr, struct lexer *lexer);

/// @brief Allocate a 'return' statement
/// @param expr The expression
/// @param lexer The lexer to use
/// @return The new statement
struct stmt_return* stmt_return_alloc(struct expr *expr, struct lexer *lexer);

/// @brief Allocate a 'block' statement
/// @param stmts An array of statements that the block consists of
/// @param stmts_len The length of stmts
/// @param stmts_cap The capacity of stmts
/// @param lexer The lexer to use
/// @return The new statement
struct stmt_block *stmt_block_alloc(struct stmt **stmts, size_t stmts_len, size_t stmts_cap, struct lexer *lexer);

/// @brief Allocate a 'mutation' statement
/// @param left The left-hand side of the mutation
/// @param right The right-hand side of the mutation
/// @param op The binary operator
/// @param lexer The lexer to use
/// @return The new statement
struct stmt_mut *stmt_mut_alloc(struct expr *left, struct expr *right, struct token *op, struct lexer *lexer);

/// @brief Allocate an 'expression' statement
/// @param expr The expression
/// @param lexer The lexer to use
/// @return The new statement
struct stmt_expr *stmt_expr_alloc(struct expr *expr, struct lexer *lexer);

/// @brief Allocate a 'fn' statement
/// @param id The ID of the function
/// @param params The parameters the function takes
/// @param params_len The length of the parameters
/// @param params_cap The capacity of the parameters
/// @param block The function body
/// @param lexer The lexer to use
/// @return The new statement
struct stmt_fn *stmt_fn_alloc(struct token *id,
                              struct token **params,
                              size_t params_len,
                              size_t params_cap,
                              struct stmt_block *block,
                              struct lexer *lexer);

/// @brief Allocate a 'fn' statement
/// @param data The actual statement
/// @param type The type of the statement
/// @param lexer The lexer to use
/// @return The new statement
struct stmt *stmt_alloc(void *data, enum stmt_type type, struct lexer *lexer);

#endif // STMT_H
