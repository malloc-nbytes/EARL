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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <any>

#include "ctx.hpp"
#include "earlty.hpp"
#include "ast.hpp"

// Struct that all expressions return.
// It contains information of the actual
// value of the expression as well as
// the type of expression that was evaluated.
struct ExprEvalResult {
  // The actual evaluated result.
  // <int, float, std::string, ... etc.>
  // or holds an ident of Token.
  std::any m_expr_value;

  // What kind of term did we encounter?
  // Integer? Identifier? etc...
  ExprTermType m_expr_term_type;

  // Given a valid ExprEvalResult, returns the
  // type as EarlTy::Type. If the type of the
  // expression is an identifier, it will do a
  // lookup in the scope to find the type of the
  // already stored variable.
  EarlTy::Type get_earl_type(Ctx &ctx);
};

void interpret(Program &program);
ExprEvalResult eval_expr(Expr *expr, Ctx &ctx);

#endif // INTERPRETER_H
