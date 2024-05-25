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

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <memory>

#include "lexer.hpp"

struct Lexer;

enum class TokenType {
  // Single character
  Lparen,
  Rparen,
  Lbracket,
  Rbracket,
  Lbrace,
  Rbrace,
  Hash,
  Period,
  Semicolon,
  Comma,
  Greaterthan,
  Lessthan,
  Equals,
  Ampersand,
  Asterisk,
  Plus,
  Minus,
  Forwardslash,
  Pipe,
  Caret,
  Questionmark,
  Backwardslash,
  Bang,
  At,
  Dollarsign,
  Percent,
  Backtick,
  Tilde,
  Colon,
  Sym_Len, // DO NOT USE! Used for the length of symbols.

  Double_Ampersand,
  Double_Pipe,
  Greaterthan_Equals,
  Lessthan_Equals,
  Double_Equals,
  Bang_Equals,
  Plus_Equals,
  Minus_Equals,
  Asterisk_Equals,
  Forwardslash_Equals,
  Percent_Equals,

  Eof,
  Intlit,
  Strlit,
  Charlit,
  Ident,
  Keyword,
  Total_Len
};

struct Token {
  std::string m_lexeme;
  TokenType m_type;
  size_t m_row;
  size_t m_col;
  std::string m_fp;
  Token *m_next;

  Token(char *start, size_t len,
        TokenType type,
        size_t row, size_t col, std::string &fp);

  const std::string &lexeme(void) const;
  TokenType type(void) const;
  std::string to_str(void);
};

Token *token_alloc(Lexer &lexer,
                   char *start, size_t len,
                   TokenType type,
                   size_t row, size_t col, std::string fp);

#endif // TOKEN_H
