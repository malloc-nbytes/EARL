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

#include "token.h"

struct expr;
struct expr_binary;
struct expr_unary;

struct expr_term;
struct expr_identifier;
struct expr_integer_literal;
struct expr_string_literal;
struct expr_character_literal;
struct expr_function_call;

struct stmt;
struct stmt_let;
struct stmt_block;
struct stmt_mut;
struct stmt_expr;
struct stmt_fn;

struct program;

/// @brief The different statement types
enum stmt_type {
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
};

/// @brief The different expression types
enum expr_type {
        EXPR_TYPE_TERM = 0,
        EXPR_TYPE_BINARY,
        EXPR_TYPE_UNARY,
};

/// @brief The different expression term types
enum expr_term_type {
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
};

//////////////////////////
// EXPRESSIONS
//////////////////////////

/// @brief Represents an expression
struct expr {
        /// @brief The type of the expression
        enum expr_type type;

        /// @brief The actual expression is contained in one of these members.
        union {

                /// @brief The expression is a term
                struct expr_term *term;

                /// @brief The expression is unary
                struct expr_unary *unary;

                /// @brief The expression is binary
                struct expr_binary *binary;
        } data;
};

/// @breif Represents a binary expression
struct expr_binary {
        /// @brief The left-hand side of the expression
        struct expr *left;

        /// @brief The right-hand side of the expression
        struct expr *right;

        /// @brief The binary operator
        struct token *op;
};

/// @brief Represents a unary expression
struct expr_unary {

        /// @brief The expression
        struct expr *expr;

        /// @brief The unary operator
        struct token *op;
};

/// @brief Represents an expression term
struct expr_term {
        /// @brief The type of the term
        enum expr_term_type type;

        /// @brief The term can be one of these
        union {
                /// @brief It is an identifier
                struct expr_identifier *identifier;

                /// @brief It is an integer literal
                struct expr_integer_literal *integer_literal;

                /// @brief It is a string literal
                struct expr_string_literal *string_literal;

                /// @brief It is a character literal
                struct expr_character_literal *character_literal;

                /// @brief It is a function call
                struct expr_function_call *function_call;
        } data;
};

/// @brief Represents an expression identifier
struct expr_identifier {
        /// @brief The token of the identifier
        struct token *identifier;
};

/// @brief Represents an expression integer literal
struct expr_integer_literal {
        /// @brief The token of the integer literal
        struct token *integer;
};

/// @brief Represents an expression string literal
struct expr_string_literal {
        /// @brief The token of the string literal
        struct token *string;
};

/// @brief Represents an expression character literal
struct expr_character_literal {
        /// @brief The token of the character literal
        struct token *character;
};

/// @brief Represents an expression function call
struct expr_function_call {
        /// @brief The left side (the callee).
        struct expr *left;

        /// @brief The arguments supplied to the function
        struct expr **args;

        /// @brief The number of arguments
        size_t args_len;

        /// @brief The capacity of the arguments
        size_t args_cap;
};

//////////////////////////
// STATEMENTS
//////////////////////////

/// @brief Represents a statement
struct stmt {
        /// @brief The type of the statement
        enum stmt_type type;

        /// @brief The actual statement is one of these
        union {
                /// @brief It is a 'let' statement
                struct stmt_let *let;

                /// @brief It is a 'block' statement
                struct stmt_block *block;

                /// @brief It is a 'mutation' statement
                struct stmt_mut *mut;

                /// @brief It is an 'expression' statement
                struct stmt_expr *expr;

                /// @brief It is a 'fn' statement
                struct stmt_fn *fn;

                /// @brief It is a 'return' statement
                struct stmt_return *ret;
        } data;
};

/// @brief Represents a 'return' statement
struct stmt_return {
        /// @brief The value that we are returning
        struct expr *expr;
};

/// @brief Represents a 'let' statement
struct stmt_let {
        /// @brief The token of the identifier
        struct token *identifier;

        /// @brief The value of the statement
        struct expr *expr;
};

/// @brief Represents a 'block' statement
struct stmt_block {
        /// @brief An array of the statements
        struct stmt **stmts;

        /// @brief The number of statements
        size_t stmts_len;

        /// @brief The capacity of the statements
        size_t stmts_cap;
};

/// @brief Represents a 'mutation' statement
struct stmt_mut {
        /// @brief The left expression
        struct expr *left;

        /// @brief The right expression
        struct expr *right;

        /// @brief The operator (=, +=, -=, etc.)
        struct token *op;
};

/// @brief Represents an 'expression' statement
struct stmt_expr {
        /// @brief The actual expression
        struct expr *expr;
};

/// @brief Represents a 'fn' statement
struct stmt_fn {
        /// @brief The identifier of the function
        struct token *id;

        /// @brief An array of identifiers for parameters
        struct token **params;

        /// @brief The number of parameters
        size_t params_len;

        /// @brief The capacity of the parameters
        size_t params_cap;

        /// @brief The function block
        struct stmt_block *block;
};

//////////////////////////
// PROGRAM
//////////////////////////

/// @brief Represents a program
struct program {
        /// @brief An array of statements
        struct stmt **stmts;

        /// @brief The number of statements
        size_t stmts_len;

        /// @brief The capacity of the statements
        size_t stmts_cap;
};

/// @brief Convert a statement type to a const char *
/// @param type The type of the statement
/// @return A string of the type
const char *stmt_type_to_cstr(enum stmt_type type);

/// @brief Print the AST of an expression
/// @param expr The expression to print
/// @param pad The padding to use
void dump_expr(const struct expr *expr, int pad);

/// @brief Print the entire AST of a program
/// @param prog The program
void ast_dump(const struct program *prog);

#endif // AST_H
