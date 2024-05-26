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

#include <algorithm>
#include <string>

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "utils.hpp"
#include "arena.hpp"
#include "token.hpp"
#include "lexer.hpp"

std::string Token::to_str(void)
{
  switch (m_type) {
  case TokenType::Lparen:
    return "LPAREN";
  case TokenType::Rparen:
    return "RPAREN";
  case TokenType::Lbracket:
    return "LBRACKET";
  case TokenType::Rbracket:
    return "RBRACKET";
  case TokenType::Lbrace:
    return "LBRACE";
  case TokenType::Rbrace:
    return "RBRACE";
  case TokenType::Hash:
    return "HASH";
  case TokenType::Period:
    return "PERIOD";
  case TokenType::Comma:
    return "COMMA";
  case TokenType::Semicolon:
    return "SEMICOLON";
  case TokenType::Greaterthan:
    return "GREATERTHAN";
  case TokenType::Lessthan:
    return "LESSTHAN";
  case TokenType::Equals:
    return "EQUALS";
  case TokenType::Ampersand:
    return "AMPERSAND";
  case TokenType::Asterisk:
    return "ASTERISK";
  case TokenType::Plus:
    return "PLUS";
  case TokenType::Minus:
    return "MINUS";
  case TokenType::Forwardslash:
    return "FORWARDSLASH";
  case TokenType::Pipe:
    return "PIPE";
  case TokenType::Caret:
    return "CARET";
  case TokenType::Questionmark:
    return "QUESTIONMARK";
  case TokenType::Backwardslash:
    return "BACKWARDSLASH";
  case TokenType::Bang:
    return "BANG";
  case TokenType::At:
    return "AT";
  case TokenType::Dollarsign:
    return "DOLLARSIGN";
  case TokenType::Percent:
    return "PERCENT";
  case TokenType::Backtick:
    return "BACKTICK";
  case TokenType::Tilde:
    return "TILDE";
  case TokenType::Colon:
    return "COLON";
  case TokenType::Double_Ampersand:
    return "DOUBLE_AMPERSAND";
  case TokenType::Double_Pipe:
    return "DOUBLE_PIPE";
  case TokenType::Greaterthan_Equals:
    return "GREATERTHAN_EQUALS";
  case TokenType::Lessthan_Equals:
    return "LESSTHAN_EQUALS";
  case TokenType::Double_Equals:
    return "DOUBLE_EQUALS";
  case TokenType::Bang_Equals:
    return "BANG_EQUALS";
  case TokenType::Plus_Equals:
    return "PLUS_EQUALS";
  case TokenType::Minus_Equals:
    return "MINUS_EQUALS";
  case TokenType::Asterisk_Equals:
    return "ASTERISK_EQUALS";
  case TokenType::Forwardslash_Equals:
    return "FORWARDSLASH_EQUALS";
  case TokenType::Percent_Equals:
    return "PERCENT_EQUALS";
  case TokenType::Eof:
    return "EOF";
  case TokenType::Intlit:
    return "INTLIT";
  case TokenType::Strlit:
    return "STRLIT";
  case TokenType::Charlit:
    return "CHARLIT";
  case TokenType::Ident:
    return "IDENT";
  case TokenType::Keyword:
    return "KEYWORD";
  case TokenType::Type:
    return "TYPE";
  default:
    assert(0 && "unknown type");
    return NULL;
  }
  return NULL;
}

Token::Token(char *start, size_t len,
             TokenType type,
             size_t row, size_t col, std::string &fp)
  : m_type(type), m_row(row), m_col(col), m_fp(std::move(fp)), m_next(nullptr) {
  std::for_each(start, start+len, [&](char c) {this->m_lexeme.push_back(c);});
}

#include <iostream>

Token *token_alloc(Lexer &lexer,
                   char *start, size_t len,
                   TokenType type,
                   size_t row, size_t col, std::string fp)
{
  Token *tok = (Token *)arena_alloc(lexer.m_arena, sizeof(Token));

  for (int i = 0; i < len; ++i) {
    tok->m_lexeme.push_back(start[i]);
  }

  // tok->m_lexeme = std::string(start, len);
  tok->m_type = type;
  tok->m_row = row;
  tok->m_col = col;
  tok->m_fp = fp;
  tok->m_next = nullptr;

  return tok;
}

const std::string &Token::lexeme(void) const {
  return m_lexeme;
}

TokenType Token::type(void) const {
  return m_type;
}
