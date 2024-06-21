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

#ifndef PARSER_H
#define PARSER_H

#include "lexer.hpp"
#include "ast.hpp"

/// @brief Provides a set of parsers as well as
/// some helper functions to parse the
/// grammar of EARL.
namespace Parser {
    /// @brief The entrypoint to parsing.
    /// @param lexer The lexer that has
    /// the linked list of tokens to parse
    std::unique_ptr<Program> parse_program(Lexer &lexer);

    /// @brief Parses a statement.
    /// @param lexer The lexer with the linked list
    /// of tokens
    std::unique_ptr<Stmt> parse_stmt(Lexer &lexer);

    /// @brief Parses a statement of type statement definition.
    /// @note ex:
    /// def my_func(a: int, b: int) -> int { ... }
    /// @param lexer The lexer with the linked list of tokens
    std::unique_ptr<StmtDef> parse_stmt_def(Lexer &lexer, uint32_t attrs);

    /// @brief Parses a statement of type
    /// statement expression. Examples of this are functions
    /// where there is no return value (or the value is to
    /// be ignored), or math equations for the REPL i.e., 1+3.
    /// @param lexer The lexer with the linked list of tokens
    std::unique_ptr<StmtExpr> parse_stmt_expr(Lexer &lexer);

    /// @brief Parses a statement of type 'let'.
    /// @note ex: let x: int = 3;
    /// @param lexer The lexer with the linked list of tokens
    std::unique_ptr<StmtLet> parse_stmt_let(Lexer &lexer);

    /// @brief Parses a statement of type mutate.
    /// @note ex: x = 3 + a * 4 / 2;
    /// @param lexer The lexer with the linked list of tokens
    std::unique_ptr<StmtMut> parse_stmt_mut(Lexer &lexer);

    /// @brief Parses a statement of type block
    /// @param lexer The lexer with the linked list of tokens
    std::unique_ptr<StmtBlock> parse_stmt_block(Lexer &lexer);

    /// @brief Parses a statement of type if.
    /// @param lexer The lexer with the linked list of tokens
    std::unique_ptr<StmtIf> parse_stmt_if(Lexer &lexer);

    /// @brief Parses an expression. It uses
    /// a recursive descent parser to determine precidence.
    /// @note The precidence is as follows:
    /// logical -> equalitative -> additive -> multiplicative -> primary.
    /// @param lexer The lexer with the linked list of tokens
    Expr *parse_expr(Lexer &lexer);

    /// @brief A utility function for the parsers
    /// to use to expect the next token to be an EARL datatype.
    /// @param lexer The lexer with the linked list of tokens
    std::unique_ptr<Token> parse_expect_type(Lexer &lexer);

    /// @brief A utility function for the parsers
    /// to use to expect the next token to be an EARL keyword.
    /// @param lexer The lexer with the linked list of tokens
    /// @param expected The keyword to expect
    std::unique_ptr<Token> parse_expect_keyword(Lexer &lexer, std::string expected);

    /// @brief A utility function for the parsers
    /// to use to expect the next token to be of type `expected`.
    /// @param lexer The lexer with the linked list of tokens
    /// @param expected The type to expect
    std::unique_ptr<Token> parse_expect(Lexer &lexer, TokenType expected);
};

#endif // PARSER_H
