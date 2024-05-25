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
#include <fstream>
#include <iostream>

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
    m_tl->m_next = std::move(tok);
    m_tl = m_tl->m_next.get();
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
  return std::move(tok);
}

void Lexer::discard(void) {
  if (!m_hd)
    return;
  m_hd.reset(m_hd->m_next);
}

void Lexer::dump(void) {
  Token *it = m_hd.get();
  while (it) {
    printf("lexeme: \"%s\", type: %s, row: %zu, col: %zu, fp: %s\n",
           it->m_lexeme.c_str(), it->to_str().c_str(), it->m_row, it->m_col, it->m_fp.c_str());
    it = it->m_next;
  }
}

static size_t consume_until(const std::string &s, const std::function<bool(char)> &predicate) {
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

std::string read_file(const std::string &filepath) {
  std::ifstream file(filepath);
  std::string content;

  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      content += line + '\n'; // Append each line to the content string
    }
    file.close();
  } else {
    std::cerr << "Unable to open file: " << filepath << std::endl;
  }

  return content;
}

static bool is_keyword(char *s, size_t len, std::vector<std::string> &keywords) {

  std::string word(s, len);

  for (std::string &kw : keywords)
    if (word == kw)
      return true;

  return false;
}

static bool issym(char c) {
  return !isalnum(c) && c != '_';
}

static bool try_comment(char *src, std::string &comment) {
  if (std::string(src).compare(0, comment.length(), comment) == 0)
    return find_comment_end(src);
  return false;
}

Lexer lex_file(char *filepath, std::vector<std::string> &keywords, std::string &comment) {
  std::string src = read_file(filepath);

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
  while (i < src.size()) {
    char *lexeme = &src[i];
    char c = src[i];

    if (c == comment[0]) {
      size_t comment_len;
      if ((comment_len = try_comment(lexeme, comment)) >= comment.size()) {
        i += comment_len;
        col = 1;
        row += 1;
        continue;
      }
    }
    else {
      lexeme = &src[i];
    }

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
      size_t strlit_len = consume_until(lexeme, [](const char c) {
        return c == '"';
      });
      std::unique_ptr<Token> tok = token_alloc(lexer, lexeme+1, strlit_len, TokenType::Strlit, row, col, filepath);
      lexer.append(std::move(tok));
      i += 1 + strlit_len + 1;
      col += 1 + strlit_len + 1;
    }

    // Character literal
    else if (c == '\'') {
      lexeme += 1;
      std::unique_ptr<Token> tok = token_alloc(lexer, lexeme, 1, TokenType::Charlit, row, col, filepath);
      lexer.append(std::move(tok));
      i += 3;
      col += 1;
    }

    // Identifers/Keywords
    else if (isalpha(c) || c == '_') {
      size_t ident_len = consume_until(lexeme, [](char c) {
        return !(c == '_' || isalnum(c));
      });
      TokenType type = is_keyword(lexeme, ident_len, keywords) ? TokenType::Keyword : TokenType::Ident;
      std::unique_ptr<Token> tok = token_alloc(lexer, lexeme, ident_len, type, row, col, filepath);
      lexer.append(std::move(tok));
      i += ident_len;
      col += ident_len;
    }

    // Numbers
    else if (isdigit(c)) {
      size_t intlit_len = consume_until(lexeme, [](char c) {
        return !isdigit(c);
      });
      std::unique_ptr<Token> tok = token_alloc(lexer, lexeme, intlit_len, TokenType::Intlit, row, col, filepath);
      lexer.append(std::move(tok));
      i += intlit_len;
      col += intlit_len;
    }

    // Symbols
    else {
      std::string buf;
      for (size_t j = 0; src[i+j] && issym(src[i+j]); ++j) {
        if (src[i+j] != ' ' && src[i+j] != '\t' && src[i+j] != '\n' && src[i+j] != '\t') {
          buf.push_back(src[i+j]);
        }
      }

      while (!buf.empty()) {
        auto it = ht.find(buf);
        if (it != ht.end()) {
          std::unique_ptr<Token> tok = token_alloc(lexer, lexeme+buf.size(), buf.size(), (*it).second, row, col, filepath);
          lexer.append(std::move(tok));
          col += buf.size();
          i += buf.size();
          break;
        }
      }
    }

  }

  return std::move(lexer);
}
