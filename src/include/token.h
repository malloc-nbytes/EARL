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

#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

/// @brief All types of tokens
typedef enum token_type {
    TOKEN_TYPE_LEFT_PARENTHESIS = 0,
    TOKEN_TYPE_RIGHT_PARENTHESIS,
    TOKEN_TYPE_LEFT_CURLY_BRACKET,
    TOKEN_TYPE_RIGHT_CURLY_BRACKET,
    TOKEN_TYPE_LEFT_SQUARE_BRACKET,
    TOKEN_TYPE_RIGHT_SQUARE_BRACKET,
    TOKEN_TYPE_HASH,
    TOKEN_TYPE_PERIOD,
    TOKEN_TYPE_SEMICOLON,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_GREATERTHAN, // 10
    TOKEN_TYPE_LESSTHAN,
    TOKEN_TYPE_EQUALS,
    TOKEN_TYPE_AMPERSAND,
    TOKEN_TYPE_ASTERISK,
    TOKEN_TYPE_PLUS,
    TOKEN_TYPE_MINUS,
    TOKEN_TYPE_FORWARD_SLASH,
    TOKEN_TYPE_PIPE,
    TOKEN_TYPE_CARET,
    TOKEN_TYPE_QUESTION_MARK, // 20
    TOKEN_TYPE_BACKWARD_SLASH,
    TOKEN_TYPE_BANG,
    TOKEN_TYPE_AT,
    TOKEN_TYPE_DOLLAR_SIGN,
    TOKEN_TYPE_PERCENT,
    TOKEN_TYPE_BACKTICK,
    TOKEN_TYPE_TILDE,
    TOKEN_TYPE_COLON,

    // Multichar
    TOKEN_TYPE_DOUBLE_AMPERSAND,
    TOKEN_TYPE_DOUBLE_PIPE, // 30
    TOKEN_TYPE_GREATERTHAN_EQUALS,
    TOKEN_TYPE_LESSTHAN_EQUALS,
    TOKEN_TYPE_DOUBLE_EQUALS,
    TOKEN_TYPE_BANG_EQUALS,
    TOKEN_TYPE_PLUS_EQUALS,
    TOKEN_TYPE_MINUS_EQUALS,
    TOKEN_TYPE_ASTERISK_EQUALS,
    TOKEN_TYPE_FORWARD_SLASH_EQUALS,
    TOKEN_TYPE_PERCENT_EQUALS,
    TOKEN_TYPE_RIGHT_ARROW, // 40
    TOKEN_TYPE_DOUBLE_PERIOD,
    TOKEN_TYPE_DOUBLE_COLON,
    TOKEN_TYPE_DOUBLE_LESSTHAN,
    TOKEN_TYPE_DOUBLE_GREATERTHAN,
    TOKEN_TYPE_DOUBLE_ASTERISK,
    TOKEN_TYPE_PIPE_GREATERTHAN,

    // Bitwise
    TOKEN_TYPE_BACKTICK_PIPE,
    TOKEN_TYPE_BACKTICK_AMPERSAND,
    TOKEN_TYPE_BACKTICK_TILDE,
    TOKEN_TYPE_BACKTICK_CARET, // 50
    TOKEN_TYPE_BACKTICK_PIPE_EQUALS,
    TOKEN_TYPE_BACKTICK_AMPERSAND_EQUALS,
    TOKEN_TYPE_BACKTICK_CARET_EQUALS,

    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_STRING_LITERAL,
    TOKEN_TYPE_INTEGER_LITERAL,
    TOKEN_TYPE_CHARACTER_LITERAL,
    TOKEN_TYPE_HEXADECIMAL_LITERAL,
    TOKEN_TYPE_FLOATING_POINT_LITERAL,
    TOKEN_TYPE_TYPE_LITERAL, // 60
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_INFO,
    TOKEN_TYPE_MULTILINE_BASH,
    TOKEN_TYPE_BASH_LITERAL,
    TOKEN_TYPE_EOF,

    TOKEN_TYPE_TOTAL_TOKEN_LEN,
} token_type_t;

/// @brief The structure of a token
typedef struct token {
    /// @brief The lexeme of the token (its value)
    char *lx;

    /// @brief The type of the token
    token_type_t type;

    /// @brief The row where the token is located
    size_t row;

    /// @brief The column where the token is located
    size_t col;

    /// @brief The next token in the linked list
    struct token *next;
} token_t;

/// @brief Allocate a token based off of
///        the lexeme `lx`.
/// @param lx The lexeme of the token
/// @param type The type of the token
/// @param row The row of the token
/// @param col The column of the token
token_t *token_alloc(const char *lx,
                     token_type_t type,
                     size_t row,
                     size_t col);

/// @brief Allocate a token based off of
///        the substring start..substr_len.
/// @param start The start of the lexeme
/// @param substr_len The length of the lexeme
/// @param type The type of the token
/// @param row The row of the token
/// @param col The column of the token
token_t *token_alloc_from_substr(const char *start,
                                 size_t substr_len,
                                 token_type_t type,
                                 size_t row,
                                 size_t col);

/// @brief Get the token type as a cstr
/// @param type The type to use
/// @return A cstr of the associated token type
const char *token_type_to_cstr(token_type_t type);

#endif // TOKEN_H
