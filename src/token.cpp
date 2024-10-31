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
#include <string>

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "err.hpp"
#include "token.hpp"
#include "lexer.hpp"

std::string
tokentype_to_str(TokenType type) {
    switch (type) {
    case TokenType::Lparen:                     return "LPAREN";
    case TokenType::Rparen:                     return "RPAREN";
    case TokenType::Lbracket:                   return "LBRACKET";
    case TokenType::Rbracket:                   return "RBRACKET";
    case TokenType::Lbrace:                     return "LBRACE";
    case TokenType::Rbrace:                     return "RBRACE";
    case TokenType::Hash:                       return "HASH";
    case TokenType::Period:                     return "PERIOD";
    case TokenType::Comma:                      return "COMMA";
    case TokenType::Semicolon:                  return "SEMICOLON";
    case TokenType::Greaterthan:                return "GREATERTHAN";
    case TokenType::Lessthan:                   return "LESSTHAN";
    case TokenType::Equals:                     return "EQUALS";
    case TokenType::Ampersand:                  return "AMPERSAND";
    case TokenType::Asterisk:                   return "ASTERISK";
    case TokenType::Plus:                       return "PLUS";
    case TokenType::Minus:                      return "MINUS";
    case TokenType::Forwardslash:               return "FORWARDSLASH";
    case TokenType::Pipe:                       return "PIPE";
    case TokenType::Caret:                      return "CARET";
    case TokenType::Questionmark:               return "QUESTIONMARK";
    case TokenType::Backwardslash:              return "BACKWARDSLASH";
    case TokenType::Bang:                       return "BANG";
    case TokenType::At:                         return "AT";
    case TokenType::Dollarsign:                 return "DOLLARSIGN";
    case TokenType::Percent:                    return "PERCENT";
    case TokenType::Backtick:                   return "BACKTICK";
    case TokenType::Tilde:                      return "TILDE";
    case TokenType::Colon:                      return "COLON";
    case TokenType::Double_Ampersand:           return "DOUBLE_AMPERSAND";
    case TokenType::Double_Pipe:                return "DOUBLE_PIPE";
    case TokenType::Greaterthan_Equals:         return "GREATERTHAN_EQUALS";
    case TokenType::Lessthan_Equals:            return "LESSTHAN_EQUALS";
    case TokenType::Double_Equals:              return "DOUBLE_EQUALS";
    case TokenType::Bang_Equals:                return "BANG_EQUALS";
    case TokenType::Plus_Equals:                return "PLUS_EQUALS";
    case TokenType::Minus_Equals:               return "MINUS_EQUALS";
    case TokenType::Asterisk_Equals:            return "ASTERISK_EQUALS";
    case TokenType::Forwardslash_Equals:        return "FORWARDSLASH_EQUALS";
    case TokenType::Percent_Equals:             return "PERCENT_EQUALS";
    case TokenType::RightArrow:                 return "RIGHTARROW";
    case TokenType::Pipe_Greaterthan:           return "PIPE_GREATERTHAN";
    case TokenType::Double_Period:              return "DOUBLE_PERIOD";
    case TokenType::Double_Colon:               return "DOUBLE_COLON";
    case TokenType::Double_Lessthan:            return "DOUBLE_LESSTHAN";
    case TokenType::Double_Greaterthan:         return "DOUBLE_GREATERTHAN";
    case TokenType::Double_Asterisk:            return "DOUBLE_ASTERISK";
    case TokenType::Backtick_Pipe:              return "BACKTICK_PIPE";
    case TokenType::Backtick_Ampersand:         return "BACKTICK_AMPERSAND";
    case TokenType::Backtick_Tilde:             return "BACKTICK_TILDE";
    case TokenType::Backtick_Caret:             return "BACKTICK_CARET";
    case TokenType::Backtick_Pipe_Equals:       return "BACKTICK_PIPE_EQUALS";
    case TokenType::Backtick_Ampersand_Equals:; return "BACKTICK_AMPERSAND_EQUALS";
    case TokenType::Backtick_Caret_Equals:;     return "BACKTICK_CARET_EQUALS";
    case TokenType::Eof:                        return "EOF";
    case TokenType::Intlit:                     return "INTLIT";
    case TokenType::Strlit:                     return "STRLIT";
    case TokenType::Charlit:                    return "CHARLIT";
    case TokenType::Floatlit:                   return "FLOATLIT";
    case TokenType::Ident:                      return "IDENTIFIER";
    case TokenType::Keyword:                    return "KEYWORD";
    case TokenType::Type:                       return "TYPE";
    case TokenType::Bashlit:                    return "BASHLIT";
    case TokenType::Info:                       return "INFO";
    case TokenType::Multiline_Bash:             return "MULTILINE_BASH";
    default:
        ERR_WARGS(Err::Type::Fatal, "unkown type: %d", static_cast<int>(type));
        return nullptr;
    }
    return nullptr;
}

Token::Token(std::string lexeme, TokenType type, size_t row, size_t col, std::string fp)
    : m_lexeme(std::move(lexeme)), m_type(type), m_row(row), m_col(col), m_fp(fp) {}

Token::Token(char *start, size_t len, TokenType type, size_t row, size_t col, std::string &fp)
    : m_type(type), m_row(row), m_col(col), m_fp(std::move(fp)), m_next(nullptr) {
    std::for_each(start, start+len, [&](char c) {this->m_lexeme.push_back(c);});
}

std::shared_ptr<Token>
token_alloc(Lexer &lexer, char *start, size_t len, TokenType type, size_t row, size_t col, std::string fp) {
    (void)lexer;
    if (type == TokenType::Strlit || type == TokenType::Bashlit || type == TokenType::Charlit) {
        std::string s = "";
        for (size_t i = 0; i < len; ++i) {
            if (i < len-3 &&
                *(start+i) == '\\' &&
                *(start+i+1) &&
                *(start+i+1) == '0' &&
                *(start+i+2) == '3' &&
                *(start+i+3) == '3') {
                s += "\033";
                i += 3;
            }
            else if (*(start+i) == '\\' && *(start+i+1) && *(start+i+1) == '\'') {
                s += '\'';
                ++i;
            }
            else if (*(start+i) == '\\' && *(start+i+1) && *(start+i+1) == 'n') {
                s += '\n';
                ++i;
            }
            else if (*(start+i) == '\\' && *(start+i+1) && *(start+i+1) == 'r') {
                s += '\r';
                ++i;
            }
            else if (*(start+i) == '\\' && *(start+i+1) && *(start+i+1) == 't') {
                s += '\t';
                ++i;
            }
            else if (*(start+i) == '\\' && *(start+i+1) && *(start+i+1) == '"') {
                s += '"';
                ++i;
            }
            else if (*(start+i) == '\\' && *(start+i+1) && *(start+i+1) == '\\') {
                s += '\\';
                ++i;
            }
            else if (*(start+i) == '\\' && *(start+i+1)) {
                auto err = std::make_shared<Token>(start, len, type, row, col, fp);
                const std::string msg = "unknown escape sequence: `\\" + std::string(1, *(start+i+1)) + '`';
                Err::err_wtok(err.get());
                throw LexerException(msg);
            }
            else {
                s += *(start+i);
            }
        }

        if (type == TokenType::Charlit && s.size() > 1) {
            auto err = std::make_shared<Token>(start, len, type, row, col, fp);
            const std::string msg = "character literals must be of size 1";
            Err::err_wtok(err.get());
            throw LexerException(msg);
        }
        return std::make_shared<Token>(s, type, row, col, fp);
    }
    return std::make_shared<Token>(start, len, type, row, col, fp);
}

std::string &
Token::lexeme(void) {
    return m_lexeme;
}

TokenType
Token::type(void) const {
    return m_type;
}

void
token_dump_until_eol(Token *tok, int padding) {
    for (int i = 0; i < padding; ++i)
        std::cout << ' ';
    while (tok && tok->type() != TokenType::Semicolon) {
        std::cout << tok->lexeme();

        if (tok->m_next && tok->m_next->type() != TokenType::Semicolon) {
            std::cout << ' ';
        }

        tok = tok->m_next.get();
    }

    std::cout << std::endl;
}

