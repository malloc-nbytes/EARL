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

#include <cassert>
#include <string>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <iostream>

#include <filesystem>

#include "err.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "utils.hpp"
#include "common.hpp"

Lexer::Lexer() : m_hd(nullptr), m_tl(nullptr), m_len(0) {}

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
        if (!tok)
            return nullptr;
        tok = tok->m_next.get();
    }
    return tok;
}

std::unique_ptr<Token> Lexer::next(void) {
    if (!m_hd)
        return nullptr;

    std::unique_ptr<Token> tok = std::move(m_hd);
    m_hd = std::move(tok->m_next);

    if (!m_hd)
        m_tl = nullptr;

    --m_len;
    return tok;
}

void Lexer::discard(void) {
    if (!m_hd)
        return;
    m_hd = std::move(m_hd->m_next);
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

static char *read_file(const char *filepath) {
    const char *search_path = "/usr/local/include/EARL/";

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
        std::filesystem::path cwd = std::filesystem::current_path();
        std::cout << "Current working directory is: " << cwd << std::endl;
        ERR_WARGS(Err::Type::Fatal, "could not find the specified source filepath: %s", filepath);
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

static bool is_keyword(char *s, size_t len, std::vector<std::string> &keywords) {

    std::string word(s, len);

    for (std::string &kw : keywords)
        if (word == kw)
            return true;

    return false;
}

static bool is_type(char *s, size_t len, std::vector<std::string> &types) {

    std::string word(s, len);

    for (std::string &ty : types)
        if (word == ty)
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

std::unique_ptr<Lexer> lex_file(const char *filepath, std::vector<std::string> &keywords, std::vector<std::string> &types, std::string &comment) {
    std::string src = read_file(filepath);

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
    };

    size_t row = 1, col = 1, i = 0;
    while (src[i]) {
        char *lexeme = &src[i];
        char c = src[i];

        if (c == comment[0]) {
            size_t comment_len;
            if ((comment_len = try_comment(lexeme, comment)) >= comment.size()) {
                while (src[i++] != '\n');
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
            size_t strlit_len = consume_until(lexeme+1, [](const char c) {
                return c == '"';
            });
            std::unique_ptr<Token> tok = token_alloc(*lexer.get(), lexeme+1, strlit_len, TokenType::Strlit, row, col, filepath);
            lexer->append(std::move(tok));
            i += 1 + strlit_len + 1;
            col += 1 + strlit_len + 1;
        }

        // Character literal
        else if (c == '\'') {
            lexeme += 1;
            std::unique_ptr<Token> tok = token_alloc(*lexer.get(), lexeme, 1, TokenType::Charlit, row, col, filepath);
            lexer->append(std::move(tok));
            i += 3;
            col += 1;
        }

        // Identifers/Keywords
        else if (isalpha(c) || c == '_') {
            size_t ident_len = consume_until(lexeme, [](char c) {
                return !(c == '_' || isalnum(c));
            });

            std::unique_ptr<Token> tok = nullptr;

            bool is_ty = is_type(lexeme, ident_len, types);
            bool is_kw = is_keyword(lexeme, ident_len, keywords);

            if (is_ty) {
                tok = token_alloc(*lexer.get(), lexeme, ident_len, TokenType::Type, row, col, filepath);
            }
            else if (is_kw) {
                tok = token_alloc(*lexer.get(), lexeme, ident_len, TokenType::Keyword, row, col, filepath);
            }
            else {
                tok = token_alloc(*lexer.get(), lexeme, ident_len, TokenType::Ident, row, col, filepath);
            }

            lexer->append(std::move(tok));
            i += ident_len;
            col += ident_len;
        }

        // Numbers
        else if (isdigit(c)) {
            size_t intlit_len = consume_until(lexeme, [](char c) {
                return !isdigit(c);
            });
            std::unique_ptr<Token> tok = token_alloc(*lexer.get(), lexeme, intlit_len, TokenType::Intlit, row, col, filepath);
            lexer->append(std::move(tok));
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

            // Pop off the buffer until we have a multichar symbol (or if we get down
            // to one element, a single char symbol).
            while (!buf.empty()) {
                auto it = ht.find(buf);
                if (it != ht.end()) {
                    std::unique_ptr<Token> tok = token_alloc(*lexer.get(), lexeme, buf.size(), (*it).second, row, col, filepath);
                    lexer->append(std::move(tok));
                    col += buf.size()-1;
                    i += buf.size();
                    break;
                }
                buf.pop_back();
            }
        }
    }

    lexer->append(token_alloc(*lexer.get(), nullptr, 0, TokenType::Eof, row, col, filepath));
    return lexer;
}
