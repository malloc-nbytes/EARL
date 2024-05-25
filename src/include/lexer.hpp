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

#include <vector>
#include <memory>

#include "arena.hpp"
#include "token.hpp"

struct Token;

struct Lexer {
  Token *m_hd;
  Token *m_tl;
  size_t m_len;

  Arena m_arena;

  Lexer();
  ~Lexer() = default;

  Token *next(void);
  Token *peek(size_t n);
  void append(Token *tok);
  void discard(void);
  void dump(void);
};

// Produces a lexer with a list of tokens from the source
// code of `filepath`. Any prevalent keywords should be provided
// in `keywords`. The identifier(s) for a SINGLE LINE comment is
// provided as `comment`.
Lexer lex_file(char *filepath, std::vector<std::string> &keywords, std::string &comment);

#endif // LEXER_H
