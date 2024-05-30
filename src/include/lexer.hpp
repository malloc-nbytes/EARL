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

// File: lexer.hpp
// Description:
//   The API for lexical analysis of
//   a document.

#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <memory>

#include "arena.hpp"
#include "token.hpp"

struct Token;

// A Lexer (for lexical analysis) https://en.wikipedia.org/wiki/Lexical_analysis
// is a tool that splits up and catagorizes individual 'tokens' for parsers.
// This implementation constructs a linked list of tokens for the parsers
// to traverse. It starts at `m_hd` and ends at `m_tl`.
struct Lexer {
  // Head and tail of the linked list
  Token *m_hd;
  Token *m_tl;

  // The number of tokens present
  size_t m_len;

  // Memory management via an arena allocator
  Arena m_arena;

  Lexer();

  // No need to free the memory for the arena
  // as it will free itself in it's destructor.
  ~Lexer() = default;

  // Get the current token, namely the one
  // that `m_hd` is currently at. It will
  // return that one and update the head accordingly.
  Token *next(void);

  // Peek `n` tokens into the lexer. This does not
  // consume the current token, only views `n` tokens ahead.
  // Returns nullptr if we peeked ahead too far.
  Token *peek(size_t n = 0);

  // Append a token to the lexer at the end of the tail.
  void append(Token *tok);

  // The same as Lexer::next() except it does not give
  // back the token that was consumed.
  void discard(void);

  // Debug function to show all tokens that were
  // lex'd.
  void dump(void);
};

// Produces a lexer with a list of tokens from the source
// code of `filepath`. Any prevalent keywords should be provided
// in `keywords`. The identifier(s) for a SINGLE LINE comment is
// provided as `comment`.
Lexer lex_file(char *filepath, std::vector<std::string> &keywords, std::vector<std::string> &types, std::string &comment);

#endif // LEXER_H
