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

#ifndef LEXER_H
#define LEXER_H

#include "misc/arena.h"
#include "lexing/token.h"

#define lexer_speek(l, i) lexer_peek((l), 0) && lexer_peek((l), 0)

extern const char *config_keywords[];

/// @brief The lexer construct.
typedef struct {
    /// @brief The head of the linked list of tokens
    token_t *hd;

    /// @brief The tail of the linked list of tokens
    token_t *tl;

    /// @brief The arena allocator to
    ///        allocate the tokens with
    arena_t arena;
} lexer_t;

/// @brief Begin lexing on the source code `src`
///        located at the filepath `fp`.
/// @param src The source code
/// @param fp The filepath that the source code is located in
/// @return A lexer that contains a linked list of tokens
lexer_t lexer_lex_src_code(const char *src, const char *fp);

/// @brief Check to see if the lexer has at least one token
/// @param lexer The lexer to check
/// @return 1 on ok, 0 on failure
int lexer_ok(const lexer_t *lexer);

/// @brief Append a token to the lexer
/// @param lexer The lexer to append to
/// @param tok The token to append
void lexer_append(lexer_t *lexer, token_t *tok);

/// @brief Print the contents of the lexer
/// @param lexer The lexer to print
void lexer_dump(lexer_t *lexer);

/// @brief Remove the head of the lexer and advance it
/// @param lexer The lexer to use
void lexer_discard(lexer_t *lexer);

/// @brief Get the head of the lexer and advance it afterwards
/// @param lexer The lexer to use
/// @return The old head of the lexer
token_t *lexer_next(lexer_t *lexer);

/// @brief Peek a token in the lexer
/// @param lexer The lexer to use
/// @param i The lookahead value
/// @return A token pointer on success, or NULL on failure
token_t *lexer_peek(lexer_t *lexer, int i);

#endif // LEXER_H
