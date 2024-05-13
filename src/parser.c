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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "notify.h"
#include "token.h"
#include "ast.h"
#include "parser.h"

struct vector(struct stmt *)
parse_stmts(struct lexer *lexer)
{
  struct vector(struct stmt *) stmts = vector_create(struct stmt *);

  struct token *curtok = NULL;
  while ((curtok = lexer_next(lexer)) != NULL) {
    switch (curtok->type) {
      case TOKENTYPE_KEYWORD: {
        if (utils_streq(curtok->lexeme, "let")) {
          ;
        }
        else if (utils_streq(curtok->lexeme, "def")) {
          ;
        }
        break;
      }
      case TOKENTYPE_IDENT: {
        break;
      }
      case TOKENTYPE_EOF:
        break;
      default: {
        NOTIFY_ERRARGS(ERR_FATAL, "parse_stmts found an unkown statement of type ID (%d).", curtok->type);
      }
    }
  }

  return stmts;
}

struct program
parse(struct lexer *lexer)
{
  return (struct program) {
    .stmts = parse_stmts(lexer),
  };
}
