// MIT License

// Copyright (c) 2023 malloc-nbytes

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

// File: lexer.h
// Description:
//   The API for lexical analysis of
//   a document.

#ifndef LEXER_H
#define LEXER_H

#include "arena.h"
#include "token.h"

struct lexer {
  // The head of the linked list.
  struct token *hd;

  // The tail of the linked list.
  struct token *tl;

  // All memory that is allocated during
  // the lexing process is alloc'd through
  // an arena allocater. See `arena.h`.
  struct arena *arena;

  // Number of tokens lex'd.
  size_t len;
};

// Produces a lexer with a list of tokens from the source
// code of `filepath`. Any prevalent keywords should be provided
// in `keywords`. The identifier(s) for a SINGLE LINE comment is
// provided as `comment`.
struct lexer lex_file(char *filepath, char **keywords, size_t keywords_len, char *comment);

// Get the next token in the lexer.
struct token *lexer_next(struct lexer *lexer);

// Append a token in the lexer.
void lexer_append(struct lexer *lexer, struct token *tok);

// Free the underlying memory in the lexer.
void lexer_free(struct lexer *lexer);

// For debugging. Print the tokens
// in a human-readable format.
void lexer_dump(struct lexer *lexer);

#endif // LEXER_H
