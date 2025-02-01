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

#ifndef AST_H
#define AST_H

#include "lexing/token.h"

typedef struct expr        expr_t;
typedef struct expr_binary expr_binary_t;
typedef struct expr_unary  expr_unary_t;

typedef struct expr_term              expr_term_t;
typedef struct expr_identifier        expr_identifier_t;
typedef struct expr_integer_literal   expr_integer_literal_t;
typedef struct expr_string_literal    expr_string_literal_t;
typedef struct expr_character_literal expr_character_literal_t;
typedef struct expr_function_call     expr_function_call_t;

typedef struct stmt        stmt_t;
typedef struct stmt_let    stmt_let_t;
typedef struct stmt_block  stmt_block_t;
typedef struct stmt_mut    stmt_mut_t;
typedef struct stmt_expr   stmt_expr_t;
typedef struct stmt_fn     stmt_fn_t;
typedef struct stmt_return stmt_return_t;

typedef struct program program_t;

/// @brief The different statement types
typedef enum {
    STMT_TYPE_FN = 0,
    STMT_TYPE_LET,
    STMT_TYPE_BLOCK,
    STMT_TYPE_MUT,
    STMT_TYPE_EXPR,
    STMT_TYPE_IF,
    STMT_TYPE_RETURN,
    STMT_TYPE_BREAK,
    STMT_TYPE_WHILE,
    STMT_TYPE_LOOP,
    STMT_TYPE_FOR,
    STMT_TYPE_FOREACH,
    STMT_TYPE_IMPORT,
    STMT_TYPE_MOD,
    STMT_TYPE_CLASS,
    STMT_TYPE_MATCH,
    STMT_TYPE_ENUM,
    STMT_TYPE_CONTINUE,
    STMT_TYPE_BASH_LITERAL,
    STMT_TYPE_INFO,
    STMT_TYPE_PIPE,
    STMT_TYPE_MULTILINE_BASH,
    STMT_TYPE_USE,
    STMT_TYPE_EXEC,
    STMT_TYPE_WITH,
} stmt_type_t;

/// @brief The different expression types
typedef enum {
    EXPR_TYPE_TERM = 0,
    EXPR_TYPE_BINARY,
    EXPR_TYPE_UNARY,
} expr_type_t;

/// @brief The different expression term types
typedef enum {
    EXPR_TERM_TYPE_IDENTIFIER = 0,
    EXPR_TERM_TYPE_INTEGER_LITERAL,
    EXPR_TERM_TYPE_STRING_LITERAL,
    EXPR_TERM_TYPE_CHARACTER_LITERAL,
    EXPR_TERM_TYPE_FUNCTION_CALL,
    EXPR_TERM_TYPE_LIST_LITERAL,
    EXPR_TERM_TYPE_RANGE,
    EXPR_TERM_TYPE_SLICE,
    EXPR_TERM_TYPE_GET,
    EXPR_TERM_TYPE_MOD_ACCESS,
    EXPR_TERM_TYPE_INDEXING,
    EXPR_TERM_TYPE_BOOL,
    EXPR_TERM_TYPE_NONE,
    EXPR_TERM_TYPE_CLOSURE,
    EXPR_TERM_TYPE_TUPLE,
    EXPR_TERM_TYPE_FLOATING_POINT_LITERAL,
    EXPR_TERM_TYPE_DICTIONARY,
    EXPR_TERM_TYPE_F_STRING,
    EXPR_TERM_TYPE_POWER,
    EXPR_TERM_TYPE_CASE,
} expr_term_type_t;

//////////////////////////
// EXPRESSIONS
//////////////////////////

/// @brief Represents an expression
typedef struct expr {
    /// @brief The type of the expression
    expr_type_t type;

    /// @brief The actual expression is contained in one of these members.
    union {
        /// @brief The expression is a term
        expr_term_t *term;

        /// @brief The expression is unary
        expr_unary_t *unary;

        /// @brief The expression is binary
        expr_binary_t *binary;
    } data;
} expr_t;

/// @brief Represents a binary expression
typedef struct expr_binary {
    /// @brief The left-hand side of the expression
    expr_t *left;

    /// @brief The right-hand side of the expression
    expr_t *right;

    /// @brief The binary operator
    token_t *op;
} expr_binary_t;

/// @brief Represents a unary expression
typedef struct expr_unary {
    /// @brief The expression
    expr_t *expr;

    /// @brief The unary operator
    token_t *op;
} expr_unary_t;

/// @brief Represents an expression term
typedef struct expr_term {
    /// @brief The type of the term
    expr_term_type_t type;

    /// @brief The term can be one of these
    union {
        /// @brief It is an identifier
        expr_identifier_t *identifier;

        /// @brief It is an integer literal
        expr_integer_literal_t *integer_literal;

        /// @brief It is a string literal
        expr_string_literal_t *string_literal;

        /// @brief It is a character literal
        expr_character_literal_t *character_literal;

        /// @brief It is a function call
        expr_function_call_t *function_call;
    } data;
} expr_term_t;

/// @brief Represents an expression identifier
typedef struct expr_identifier {
    /// @brief The token of the identifier
    token_t *identifier;
} expr_identifier_t;

/// @brief Represents an expression integer literal
typedef struct expr_integer_literal {
    /// @brief The token of the integer literal
    token_t *integer;
} expr_integer_literal_t;

/// @brief Represents an expression string literal
typedef struct expr_string_literal {
    /// @brief The token of the string literal
    token_t *string;
} expr_string_literal_t;

/// @brief Represents an expression character literal
typedef struct expr_character_literal {
    /// @brief The token of the character literal
    token_t *character;
} expr_character_literal_t;

/// @brief Represents an expression function call
typedef struct expr_function_call {
    /// @brief The left side (the callee).
    expr_t *left;

    /// @brief The arguments supplied to the function
    expr_t **args;

    /// @brief The number of arguments
    size_t args_len;

    /// @brief The capacity of the arguments
    size_t args_cap;
} expr_function_call_t;

//////////////////////////
// STATEMENTS
//////////////////////////

/// @brief Represents a statement
typedef struct stmt {
    /// @brief The type of the statement
    stmt_type_t type;

    /// @brief The actual statement is one of these
    union {
        /// @brief It is a 'let' statement
        stmt_let_t *let;

        /// @brief It is a 'block' statement
        stmt_block_t *block;

        /// @brief It is a 'mutation' statement
        stmt_mut_t *mut;

        /// @brief It is an 'expression' statement
        stmt_expr_t *expr;

        /// @brief It is a 'fn' statement
        stmt_fn_t *fn;

        /// @brief It is a 'return' statement
        stmt_return_t *ret;
    } data;
} stmt_t;

/// @brief Represents a 'return' statement
typedef struct stmt_return {
    /// @brief The value that we are returning
    expr_t *expr;
} stmt_return_t;

/// @brief Represents a 'let' statement
typedef struct stmt_let {
    /// @brief The token of the identifier
    token_t *identifier;

    /// @brief The value of the statement
    expr_t *expr;
} stmt_let_t;

/// @brief Represents a 'block' statement
typedef struct stmt_block {
    /// @brief An array of the statements
    stmt_t **stmts;

    /// @brief The number of statements
    size_t stmts_len;

    /// @brief The capacity of the statements
    size_t stmts_cap;
} stmt_block_t;

/// @brief Represents a 'mutation' statement
typedef struct stmt_mut {
    /// @brief The left expression
    expr_t *left;

    /// @brief The right expression
    expr_t *right;

    /// @brief The operator (=, +=, -=, etc.)
    token_t *op;
} stmt_mut_t;

/// @brief Represents an 'expression' statement
typedef struct stmt_expr {
    /// @brief The actual expression
    expr_t *expr;
} stmt_expr;

/// @brief Represents a 'fn' statement
typedef struct stmt_fn {
    /// @brief The identifier of the function
    token_t *id;

    /// @brief The parameters of the function
    struct {
        /// @brief An array of identifiers for parameters
        token_t **data;

        /// @brief The length of parameters
        size_t len;

        /// @brief The capacity of the parameters
        size_t cap;
    } params;

    /// @brief The function block
    stmt_block_t *block;
} stmt_fn_t;

//////////////////////////
// PROGRAM
//////////////////////////

/// @brief Represents a program
typedef struct program {
    /// @brief An array of statements
    stmt_t **stmts;

    /// @brief The number of statements
    size_t stmts_len;

    /// @brief The capacity of the statements
    size_t stmts_cap;
} program_t;

/// @brief Convert a statement type to a const char *
/// @param type The type of the statement
/// @return A string of the type
const char *stmt_type_to_cstr(stmt_type_t type);

/// @brief Print the AST of an expression
/// @param expr The expression to print
/// @param pad The padding to use
void dump_expr(const expr_t *expr, int pad);

/// @brief Print the entire AST of a program
/// @param prog The program
void ast_dump(const program_t *prog);

#endif // AST_H
