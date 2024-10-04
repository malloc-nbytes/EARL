/** @file */

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

#include <iostream>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <functional>

#include "err.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "utils.hpp"
#include "common.hpp"
#include "config.h"

Lexer::Lexer() : m_hd(nullptr), m_tl(nullptr), m_len(0) {}

void Lexer::append(std::shared_ptr<Token> tok) {
    if (!m_hd) {
        m_hd = tok;
        m_tl = m_hd.get();
    } else {
        m_tl->m_next = tok;
        m_tl = m_tl->m_next.get();
    }
    ++m_len;
}

void Lexer::append(std::string lexeme, TokenType type, size_t row, size_t col, std::string fp) {
    auto tok = std::make_shared<Token>(lexeme, type, row, col, fp);
    this->append(std::move(tok));
}

Token *Lexer::peek(size_t n) {
    Token *tok = m_hd.get();
    for (size_t i = 0; i < n && tok; ++i) {
        if (!tok)
            return nullptr;
        tok = tok->m_next.get();
    }
    return tok;
}

std::shared_ptr<Token> Lexer::next(void) {
    if (!m_hd)
        return nullptr;

    std::shared_ptr<Token> tok = m_hd;
    m_hd = tok->m_next;

    if (!m_hd)
        m_tl = nullptr;

    --m_len;
    return tok;
}

void Lexer::discard(void) {
    if (!m_hd)
        return;
    m_hd = m_hd->m_next;
}

void Lexer::dump(void) {
    Token *it = m_hd.get();
    while (it) {
        printf("lexeme: \"%s\", type: %s, row: %zu, col: %zu, fp: %s\n",
               it->m_lexeme.c_str(), tokentype_to_str(it->type()).c_str(), it->m_row, it->m_col, it->m_fp.c_str());
        it = it->m_next.get();
    }
}

static size_t consume_until(const std::string &s, const std::function<bool(char)> &predicate) {
    size_t i;
    bool skip = false;
    for (i = 0; s[i]; ++i) {
        if (!skip && predicate(s[i]))
            return i;
        if (s[i] == '\\')
            skip = true;
        else
            skip = false;
    }
    return i;
}

static size_t find_comment_end(char *s) {
    size_t i;
    for (i = 0; s[i]; ++i)
        if (s[i] == '\n')
            return i;
    return i;
}

static bool
is_keyword(char *s, size_t len, std::vector<std::string> &keywords) {
    std::string word(s, len);
    for (std::string &kw : keywords)
        if (word == kw)
            return true;
    return false;
}

static bool
is_type(char *s, size_t len, std::vector<std::string> &types) {
    std::string word(s, len);
    for (std::string &ty : types)
        if (word == ty)
            return true;
    return false;
}

static bool
issym(char c) {
    return !isalnum(c) && c != '_';
}

static bool
try_comment(char *src, std::string &comment) {
    if (std::string(src).compare(0, comment.length(), comment) == 0)
        return find_comment_end(src);
    return false;
}

char *
read_file(const char *filepath) {
    const char *search_path = PREFIX "/include/EARL/";

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s", search_path, filepath);

    FILE *f = nullptr;

    if ((flags & __WITHOUT_STDLIB) == 0) {
        f = fopen(full_path, "rb");
    }

    if (f == nullptr) {
        f = fopen(filepath, "rb");
    }

    if (f == nullptr || fseek(f, 0, SEEK_END)) {
        std::string msg = "could not find the specified source filepath: " + std::string(filepath);
        throw std::runtime_error(msg);
    }

    long length = ftell(f);
    rewind(f);
    if (length == -1 || (unsigned long) length >= SIZE_MAX) {
        fclose(f);
        return NULL;
    }

    size_t ulength = (size_t)length;
    char *buffer = (char *)malloc(ulength + 1);

    if (buffer == NULL || fread(buffer, 1, ulength, f) != ulength) {
        fclose(f);
        free(buffer);
        return NULL;
    }
    buffer[ulength] = '\0';

    fclose(f);
    return buffer;
}

std::unique_ptr<Lexer>
lex_file(std::string &src,
         std::string fp,
         std::vector<std::string> &keywords,
         std::vector<std::string> &types,
         std::string &comment) {
    (void)is_keyword;
    (void)is_type;
    (void)issym;
    (void)try_comment;
    (void)types;
    (void)comment;
    std::unique_ptr<Lexer> lexer = std::make_unique<Lexer>();

    const std::unordered_map<std::string, TokenType> ht = {
        {"(", TokenType::Lparen},
        {")", TokenType::Rparen},
        {"[", TokenType::Lbracket},
        {"]", TokenType::Rbracket},
        {"{", TokenType::Lbrace},
        {"}", TokenType::Rbrace},
        {"#", TokenType::Hash},
        {".", TokenType::Period},
        {";", TokenType::Semicolon},
        {",", TokenType::Comma},
        {">", TokenType::Greaterthan},
        {"<", TokenType::Lessthan},
        {"=", TokenType::Equals},
        {"&", TokenType::Ampersand},
        {"*", TokenType::Asterisk},
        {"+", TokenType::Plus},
        {"-", TokenType::Minus},
        {"/", TokenType::Forwardslash},
        {"|", TokenType::Pipe},
        {"^", TokenType::Caret},
        {"?", TokenType::Questionmark},
        {"\\", TokenType::Backwardslash},
        {"!", TokenType::Bang},
        {"@", TokenType::At},
        {"$", TokenType::Dollarsign},
        {"%", TokenType::Percent},
        {"`", TokenType::Backtick},
        {"~", TokenType::Tilde},
        {":", TokenType::Colon},
        {"&&", TokenType::Double_Ampersand},
        {"||", TokenType::Double_Pipe},
        {">=", TokenType::Greaterthan_Equals},
        {"<=", TokenType::Lessthan_Equals},
        {"==", TokenType::Double_Equals},
        {"!=", TokenType::Bang_Equals},
        {"+=", TokenType::Plus_Equals},
        {"-=", TokenType::Minus_Equals},
        {"*=", TokenType::Asterisk_Equals},
        {"/=", TokenType::Forwardslash_Equals},
        {"%=", TokenType::Percent_Equals},
        {"->", TokenType::RightArrow},
        {"..", TokenType::Double_Period},
        {"::", TokenType::Double_Colon},
        {"**", TokenType::Double_Asterisk},
        {"<<", TokenType::Double_Lessthan},
        {">>", TokenType::Double_Greaterthan},
        {"`|", TokenType::Backtick_Pipe},
        {"`&", TokenType::Backtick_Ampersand},
        {"`~", TokenType::Backtick_Tilde},
        {"`^", TokenType::Backtick_Caret},
        {"`|=", TokenType::Backtick_Pipe_Equals},
        {"`&=", TokenType::Backtick_Ampersand_Equals},
        {"`^=", TokenType::Backtick_Caret_Equals},
    };

    int row = 1, col = 0;
    size_t i = 0;
    while (i < src.size()) {
        char *lexeme = &src[i];

        if (i < src.size()-2 && src[i] == '#' && src[i+1] == '#' && src[i+2] == '#') {
            std::string info = "";
            i += 3;
            while (src[i] && src[i] != '\n') {
                info += src[i];
                ++col, ++i;
            }

            while (info[0] == ' ')
                info.erase(info.begin());

            lexer->append(info, TokenType::Info, row, col, fp);
        }

        if (src[i] == '#') {
            while (src[i] != '\n') {
                ++i;
                ++col;
            }
        }

        else if (src[i] == '\t' || src[i] == ' ') {
            ++i;
            ++col;
        }

        else if (src[i] == '\n') {
            col = 0;
            ++row;
            ++i;
        }

        else if (src[i] == '"') {
            size_t strlit_len = consume_until(lexeme+1, [](const char c) {
                return c == '"';
            });
            std::shared_ptr<Token> tok = token_alloc(*lexer.get(), lexeme+1, strlit_len, TokenType::Strlit, row, col, fp);
            lexer->append(std::move(tok));
            i += 1 + strlit_len + 1;
            col += 1 + strlit_len + 1;
        }

        else if (src[i] == '\'') {
            std::string charlit = "";
            ++i;
            if (src[i] == '\\') {
                ++i;
                ++col;
                std::string escape = "\\" + std::string(1, src[i]);
                charlit = escape;
            }
            else
                charlit = std::string(1, src[i]);
            lexer->append(charlit, TokenType::Charlit, row, col, fp);
            i += 2;
            col += 3;
        }

        else if (isalpha(src[i]) || src[i] == '_') {
            std::string ident = "";
            while (src[i] == '_' || isalnum(src[i]))
                ident += src[i++];
            if (std::find(keywords.begin(), keywords.end(), ident) != keywords.end())
                lexer->append(ident, TokenType::Keyword, row, col+1, fp);
            else
                lexer->append(ident, TokenType::Ident, row, col+1, fp);
            col += ident.size()+1;
        }

        else if (isdigit(src[i])) {
            std::string digit = "";
            while (isdigit(src[i]))
                digit += src[i++];
            if (src[i] && src[i+1] && src[i] == '.' && src[i+1] != '.') {
                ++i;
                std::string digit2 = ".";
                while (isdigit(src[i]))
                    digit2 += src[i++];
                lexer->append(digit+digit2, TokenType::Floatlit, row, col, fp);
                // no need for +1 for `.` because its in `digit2`
                col += digit.size()+digit2.size();
            }
            else {
                lexer->append(digit, TokenType::Intlit, row, col, fp);
                col += digit.size()+1;
            }
        }

        else {
            std::string buf = "";
            while (src[i] && !isalnum(src[i]) && src[i] != '_')
                buf += src[i++];
            while (!buf.empty()) {
                auto it = ht.find(buf);
                if (it != ht.end()) {
                    if (buf == "." && src[i] && isdigit(src[i])) {
                        std::string digit = "";
                        while (isdigit(src[i]))
                            digit += src[i++];
                        lexer->append(buf+digit, TokenType::Floatlit, row, col, fp);
                        col += digit.size()+1;
                    }
                    else
                        lexer->append(buf, (*it).second, row, col, fp);
                    break;
                }
                else {
                    buf.pop_back();
                    --i;
                }
            }
        }
    }

    lexer->append(token_alloc(*lexer.get(), nullptr, 0, TokenType::Eof, row, col, fp));

    if ((flags & __VERBOSE) != 0)
        std::cout << "[EARL] lex'd file " << fp << " (#tokens=" << lexer->m_len << ")" << std::endl;

    return lexer;
}
