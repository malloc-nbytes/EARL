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

/**
 * Provides all of the possible token types in EARL
 * as well as define what a `Token` is.
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <memory>

//#include "lexer.hpp"

struct Lexer;

enum class TokenType {
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
    Greaterthan, // 10
    Lessthan,
    Equals,
    Ampersand,
    Asterisk,
    Plus,
    Minus,
    Forwardslash,
    Pipe,
    Caret,
    Questionmark, // 20
    Backwardslash,
    Bang,
    At,
    Dollarsign,
    Percent,
    Backtick,
    Tilde,
    Colon,
    Sym_Len, // DO NOT USE! Used for the length of symbols. (deprecated)

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
    RightArrow,
    Double_Period,
    Double_Colon,
    Double_Lessthan,
    Double_Greaterthan,
    Double_Asterisk,

    // Bitwise
    Backtick_Pipe,
    Backtick_Ampersand,
    Backtick_Tilde,
    Backtick_Caret,

    Backtick_Pipe_Equals,
    Backtick_Ampersand_Equals,
    Backtick_Caret_Equals,

    // Other
    Eof,
    Intlit,
    Strlit,
    Charlit,
    Floatlit,
    Ident,
    Keyword,
    Type,
    Total_Len
};

std::string tokentype_to_str(TokenType type);

/// @brief The definition of a token.
struct Token {
    Token(char *start, size_t len,
          TokenType type,
          size_t row, size_t col, std::string &fp);

    Token(std::string lexeme, TokenType type, size_t row, size_t col, std::string fp);

    Token(const Token &) = delete;

    ~Token() = default;

    /// @brief The actual value of the `Token`
    std::string m_lexeme;

    /// @brief The type of the token
    TokenType m_type;

    /// @brief The row of the token
    size_t m_row;

    /// @brief The column of the token
    size_t m_col;

    /// @brief The filepath of the token
    std::string m_fp;

    /// @brief A pointer to the next token
    std::shared_ptr<Token> m_next;

    /// @brief Get the `lexeme` of the current token
    std::string &lexeme(void);

    /// @brief Get the `type` of the current token
    TokenType type(void) const;
};

/// @brief Allocate a `Token` using an `Arena Allocator`
/// @param lexer The lexer that is currently being used. It is needed for it's `arena`.
/// @param start A pointer to the start of the lexeme to create
/// @param len How many characters for the lexeme
/// @param type The type of the token to create
/// @param row The row of the token
/// @param col The column of the token
/// @param fp The filepath of the token
std::shared_ptr<Token> token_alloc(Lexer &lexer, char *start, size_t len, TokenType type, size_t row, size_t col, std::string fp);

/// @brief Prints tokens from the current token to the end of line.
/// @param tok The token to start from
/// @param padding The number of spaces to prepend the output
void token_dump_until_eol(Token *tok, int padding = 2);

#endif // TOKEN_H
