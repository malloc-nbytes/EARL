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

#include "lexer.h"
#include "ast.h"

// Parsers
struct program parser_parse(struct lexer *lexer);
struct stmt *parser_parse_stmt(struct lexer *lexer);
struct stmt_let *parser_parse_stmt_let(struct lexer *lexer);

struct vector(struct pair(struct token *id, struct token *type))
       parser_parse_stmt_def_args(struct lexer *lexer);

struct stmt_block *parser_parse_stmt_block(struct lexer *lexer);
struct stmt_def *parser_parse_stmt_def(struct lexer *lexer);

// Expression Parsers
struct expr *parser_parse_expr(struct lexer *lexer);

// Helpers
struct token *parser_expect(struct lexer *lexer, enum token_type exp);
struct token *parser_expect_keyword(struct lexer *lexer, const char *keyword);
struct token *parser_expect_type(struct lexer *lexer);

#endif // PARSER_H
