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

// File: parser.hpp
// Description:
//   Provides a set of parsers as well as
//   some helper functions to parse the
//   grammar of EARL.

#ifndef PARSER_H
#define PARSER_H

#include "lexer.hpp"
#include "ast.hpp"

namespace Parser {
    // The entrypoint to parsing. Takes a lexer
    // `lexer` and produces a Program.
    Program parse_program(Lexer &lexer);

    // Given `lexer`, parses a statement.
    std::unique_ptr<Stmt> parse_stmt(Lexer &lexer);

    // Given `lexer`, parses a statement of type
    // statement definition.
    // ex:
    //     def my_func(a: int, b: int) -> int { ... }
    std::unique_ptr<StmtDef> parse_stmt_def(Lexer &lexer);

    // Given `lexer` parses a statement of type
    // statement expression. Examples of this are functions
    // where there is no return value (or the value is to
    // be ignored), or math equations for the REPL i.e., 1+3.
    std::unique_ptr<StmtExpr> parse_stmt_expr(Lexer &lexer);

    // Given `lexer`, parses a statement of type 'let'.
    // ex:
    //   let x: int = 3;
    std::unique_ptr<StmtLet> parse_stmt_let(Lexer &lexer);

    // Given `lexer`, parses a statement of type mutate.
    // ex:
    //   x = 3 + a * 4 / 2;
    std::unique_ptr<StmtMut> parse_stmt_mut(Lexer &lexer);

    // Given `lexer`, parses an expression. It uses
    // a recursive descent parser to determine precidence.
    // The precidence is as follows:
    //   logical -> equalitative -> additive -> multiplicative -> primary.
    Expr *parse_expr(Lexer &lexer);

    // Given `lexer`, a utility function for the parsers
    // to use to expect the next token to be an EARL datatype.
    Token *parse_expect_type(Lexer &lexer);

    // Given `lexer`, a utility function for the parsers
    // to use to expect the next token to be an EARL keyword.
    Token *parse_expect_keyword(Lexer &lexer, std::string expected);

    // Given `lexer`, a utility function for the parsers
    // to use to expect the next token to be of type `expected`.
    Token *parse_expect(Lexer &lexer, TokenType expected);
};

#endif // PARSER_H
