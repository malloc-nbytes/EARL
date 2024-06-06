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

#include "err.hpp"
#include "utils.hpp"
#include "arena.hpp"
#include "token.hpp"
#include "lexer.hpp"

std::string Token::to_str(void)
{
    switch (m_type) {
    case TokenType::Lparen:
        return "`(`";
    case TokenType::Rparen:
        return "`)`";
    case TokenType::Lbracket:
        return "`[`";
    case TokenType::Rbracket:
        return "`]`";
    case TokenType::Lbrace:
        return "`{`";
    case TokenType::Rbrace:
        return "`}`";
    case TokenType::Hash:
        return "`#`";
    case TokenType::Period:
        return "`.`";
    case TokenType::Comma:
        return "`,`";
    case TokenType::Semicolon:
        return "`;`";
    case TokenType::Greaterthan:
        return "`>`";
    case TokenType::Lessthan:
        return "`<`";
    case TokenType::Equals:
        return "`=`";
    case TokenType::Ampersand:
        return "`&`";
    case TokenType::Asterisk:
        return "`*`";
    case TokenType::Plus:
        return "`+`";
    case TokenType::Minus:
        return "`-`";
    case TokenType::Forwardslash:
        return "`/`";
    case TokenType::Pipe:
        return "`|`";
    case TokenType::Caret:
        return "`^`";
    case TokenType::Questionmark:
        return "`?`";
    case TokenType::Backwardslash:
        return "`\\`";
    case TokenType::Bang:
        return "`!`";
    case TokenType::At:
        return "`@`";
    case TokenType::Dollarsign:
        return "`$`";
    case TokenType::Percent:
        return "`@`";
    case TokenType::Backtick:
        return "```";
    case TokenType::Tilde:
        return "`~`";
    case TokenType::Colon:
        return "`:`";
    case TokenType::Double_Ampersand:
        return "`&&`";
    case TokenType::Double_Pipe:
        return "`||`";
    case TokenType::Greaterthan_Equals:
        return "`>=`";
    case TokenType::Lessthan_Equals:
        return "`<=`";
    case TokenType::Double_Equals:
        return "`==`";
    case TokenType::Bang_Equals:
        return "`!=`";
    case TokenType::Plus_Equals:
        return "`+=`";
    case TokenType::Minus_Equals:
        return "`-=`";
    case TokenType::Asterisk_Equals:
        return "`*=`";
    case TokenType::Forwardslash_Equals:
        return "`/=`";
    case TokenType::Percent_Equals:
        return "`%=`";
    case TokenType::RightArrow:
        return "`->`";
    case TokenType::Double_Period:
        return "`..`";
    case TokenType::Eof:
        return "`EOF`";
    case TokenType::Intlit:
        return "`INTLIT`";
    case TokenType::Strlit:
        return "`STRLIT`";
    case TokenType::Charlit:
        return "`CHARLIT`";
    case TokenType::Ident:
        return "`IDENTIFIER`";
    case TokenType::Keyword:
        return "`KEYWORD`";
    case TokenType::Type:
        return "`TYPE`";
    default:
        ERR_WARGS(Err::Type::Fatal, "unkown type: %d", static_cast<int>(m_type));
        return nullptr;
    }
    return nullptr;
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

    for (size_t i = 0; i < len; ++i) {
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

std::string &Token::lexeme(void) {
    return m_lexeme;
}

TokenType Token::type(void) const {
    return m_type;
}
