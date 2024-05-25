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

#include <cassert>
#include <string>
#include <unordered_map>
#include <functional>

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "notify.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "utils.hpp"
#include "arena.hpp"

Lexer::Lexer(Lexer&& other) noexcept
    : m_hd(std::move(other.m_hd)), m_tl(other.m_tl),
      m_len(other.m_len), m_arena(std::move(other.m_arena)) {
  other.m_tl = nullptr;
  other.m_len = 0;
}

Lexer& Lexer::operator=(Lexer&& other) noexcept {
  if (this != &other) {
    m_hd = std::move(other.m_hd);
    m_tl = other.m_tl;
    m_len = other.m_len;
    m_arena = std::move(other.m_arena);

    other.m_tl = nullptr;
    other.m_len = 0;
  }
  return *this;
}

Lexer::Lexer() : m_arena(32768) {
  m_len = 0;
}

void Lexer::append(std::unique_ptr<Token> tok) {
  if (!m_hd) {
    m_hd = std::move(tok);
    m_tl = m_hd.get();
  } else {
    m_tl->m_next = tok.get();
    m_tl = tok.get();
  }
  ++m_len;
}

Token *Lexer::peek(size_t n) {
  Token *tok = m_hd.get();
  for (size_t i = 0; i < n && tok; ++i) {
    tok = tok->m_next;
  }
  return tok;
}

std::unique_ptr<Token> Lexer::next(void) {
  if (!m_hd)
    return nullptr;

  std::unique_ptr<Token> tok = std::move(m_hd);
  m_hd.reset(tok->m_next);

  if (!m_hd)
    m_tl = nullptr;

  --m_len;
  return tok;
}

void Lexer::discard(void) {
  assert(false && "todo");
}

void Lexer::dump(void) {
  assert(false && "todo");
}

static size_t consume_until(std::string &s, std::function<bool(char)> &predicate) {
  size_t i;
  bool skip = false;
  for (i = 0; s[i]; ++i) {
    if (!skip && predicate(s[i])) {
      return i;
    }
    if (s[i] == '\\') {
      skip = true;
    }
    else {
      skip = false;
    }
  }
  return i;
}

static size_t find_comment_end(char *s) {
  size_t i;
  for (i = 0; s[i]; ++i) {
    if (s[i] == '\n') {
      return i;
    }
  }
  return i;
}

static int is_newline(char c) {
  return c == '\n';
}

static int is_quote(char c) {
  return c == '"';
}

static int nisdigit(char c) {
  return !isdigit(c);
}

static int nisvalid_ident(char c) {
  return !(c == '_' || isalnum(c));
}

// Code from:
//   chux - Reinstate Monica
//   https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
static char * file_to_str(char *filepath) {
  FILE *f = fopen(filepath, "rb");

  if (f == NULL || fseek(f, 0, SEEK_END)) {
    return NULL;
  }

  long length = ftell(f);
  rewind(f);
  if (length == -1 || (unsigned long) length >= SIZE_MAX) {
    return NULL;
  }

  size_t ulength = (size_t)length;
  char *buffer = static_cast<char *>(utils_safe_malloc(ulength+1));

  if (buffer == NULL || fread(buffer, 1, ulength, f) != ulength) {
    free(buffer);
    return NULL;
  }
  buffer[ulength] = '\0';

  return buffer;
}

static int is_keyword(char *s, size_t len, char **keywords, size_t keywords_len) {
  // TODO: possible stack overflow.
  char real[256];
  memset(real, '\0', len);
  memcpy(real, s, len);

  for (size_t i = 0; i < keywords_len; ++i) {
    if (utils_streq(real, keywords[i])) {
      return 1;
    }
  }

  return 0;
}

static int issym(char c) {
  return !isalnum(c) && c != '_';
}

int try_comment(char *src, char *comment) {
  if (strncmp(src, comment, strlen(comment)) == 0) {
    return find_comment_end(src);
  }

  return 0;
}

Lexer lex_file(char *filepath, std::vector<std::string> &keywords, std::string &comment) {
  std::string src = std::string(file_to_str(filepath));

  Lexer lexer;

  std::unordered_map<std::string, TokenType> ht;

  ht["("] = TokenType::Lparen;
  ht[")"] = TokenType::Rparen;
  ht["["] = TokenType::Lbracket;
  ht["]"] = TokenType::Rbracket;
  ht["{"] = TokenType::Lbrace;
  ht["}"] = TokenType::Rbrace;
  ht["#"] = TokenType::Hash;
  ht["."] = TokenType::Period;
  ht[";"] = TokenType::Semicolon;
  ht[","] = TokenType::Comma;
  ht[">"] = TokenType::Greaterthan;
  ht["<"] = TokenType::Lessthan;
  ht["="] = TokenType::Equals;
  ht["&"] = TokenType::Ampersand;
  ht["*"] = TokenType::Asterisk;
  ht["+"] = TokenType::Plus;
  ht["-"] = TokenType::Minus;
  ht["/"] = TokenType::Forwardslash;
  ht["|"] = TokenType::Pipe;
  ht["^"] = TokenType::Caret;
  ht["?"] = TokenType::Questionmark;
  ht["\\"] = TokenType::Backwardslash;
  ht["!"] = TokenType::Bang;
  ht["@"] = TokenType::At;
  ht["$"] = TokenType::Dollarsign;
  ht["%"] = TokenType::Percent;
  ht["`"] = TokenType::Backtick;
  ht["~"] = TokenType::Tilde;
  ht[":"] = TokenType::Colon;
  ht["&&"] = TokenType::Double_Ampersand;
  ht["||"] = TokenType::Double_Pipe;
  ht[">="] = TokenType::Greaterthan_Equals;
  ht["<="] = TokenType::Lessthan_Equals;
  ht["=="] = TokenType::Double_Equals;
  ht["!="] = TokenType::Bang_Equals;
  ht["+="] = TokenType::Plus_Equals;
  ht["-="] = TokenType::Minus_Equals;
  ht["*="] = TokenType::Asterisk_Equals;
  ht["/="] = TokenType::Forwardslash_Equals;
  ht["%="] = TokenType::Percent_Equals;

  size_t row = 0, col = 0, i = 0;
  while (src[i]) {
    char c = src[i];

    // Newlines
    if (c == '\r' || c == '\n') {
      ++row;
      col = 1;
      ++i;
    }

    // Tabs/empty spaces
    else if (c == '\t' || c == ' ') {
      ++col;
      ++i;
    }

    // String literal
    else if (c == '"') {
      assert(false && "unimplemented");
    }

    // Character literal
    else if (c == '\'') {
      assert(false && "unimplemented");
    }

    // Identifers/Keywords
    else if (isalpha(c) || c == '_') {
      assert(false && "unimplemented");
    }

    // Numbers
    else if (isdigit(c)) {
      assert(false && "unimplemented");
    }

    // Symbols
    else {
      assert(false && "unimplemented");
    }

  }

  return std::move(lexer);
}
