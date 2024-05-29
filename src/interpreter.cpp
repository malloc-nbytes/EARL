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

#include <any>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>

#include "interpreter.hpp"
#include "err.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "ctx.hpp"
#include "earlvar.hpp"
#include "common.hpp"

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
  EarlTy::Type get_earl_type(Ctx &ctx) {
    if (m_expr_term_type == ExprTermType::Ident) {
      Token *tok = std::any_cast<Token *>(m_expr_value);
      if (!ctx.earlvar_in_scope(tok->lexeme())) {
        ERR_WARGS(ErrType::Fatal, "variable `%s` is not in scope", tok->lexeme().c_str());
      }
      EarlVar &var = ctx.get_earlvar_from_scope(tok->lexeme());
      return var.m_type;
    }

    switch (m_expr_term_type) {
    case ExprTermType::Int_Literal:
      return EarlTy::Type::Int;
    case ExprTermType::Str_Literal:
      return EarlTy::Type::Str;
    default:
      ERR_WARGS(ErrType::Fatal, "ExprTermType `%d` is not a valid EARL type", static_cast<int>(m_expr_term_type));
    }
  }
};

ExprEvalResult eval_expr_term(ExprTerm *expr) {
  switch (expr->get_term_type()) {
  case ExprTermType::Ident: {
    ExprIdent *ident = dynamic_cast<ExprIdent *>(expr);
    return ExprEvalResult {ident->m_tok.get(), ident->get_term_type()};
  } break;
  case ExprTermType::Int_Literal: {
    ExprIntLit *intlit = dynamic_cast<ExprIntLit *>(expr);
    return ExprEvalResult {std::stoi(intlit->m_tok->lexeme()), intlit->get_term_type()};
  } break;
  case ExprTermType::Str_Literal: {
    assert(false && "unimplemented");
  } break;
  default:
    ERR_WARGS(ErrType::Fatal, "%d is not a valid expression term type is not valid",
              static_cast<int>(expr->get_term_type()));
  }
}

ExprEvalResult eval_expr_bin(ExprBinary *expr) {
  assert(false && "unimplemented");
  (void)expr;
  return ExprEvalResult {};
}

ExprEvalResult eval_expr(Expr *expr) {
  switch (expr->get_type()) {
  case ExprType::Term: {
    return eval_expr_term(dynamic_cast<ExprTerm *>(expr));
  } break;
  case ExprType::Binary: {
    assert(false && "unimplemented");
  } break;
  case ExprType::Func_Call: {
    assert(false && "unimplemented");
  } break;
  default:
    ERR_WARGS(ErrType::Fatal, "expression type %d is not a valid expression",
              static_cast<int>(expr->get_type()));
  }
}

void eval_stmt_let(StmtLet *stmt, Ctx &ctx) {
  const std::string &id = stmt->m_id->lexeme();
  if (ctx.earlvar_in_scope(id)) {
    ERR_WARGS(ErrType::Redeclared, "variable `%s` is already defined", id.c_str());
  }

  // The `let` type binding i.e., let x: <TYPE> = ...;
  EarlTy::Type binding_type = EarlTy::of_str(stmt->m_type->lexeme());

  ExprEvalResult expr_eval = eval_expr(stmt->m_expr.get());

  // The type of the right side of the equals sign
  EarlTy::Type rval_type = expr_eval.get_earl_type(ctx);

  if (!EarlTy::earlvar_type_compat(binding_type, rval_type)) {
    ERR_WARGS(ErrType::ERR_FATAL, "type (%d) is not compatable with type (%d)",
              static_cast<int>(binding_type), static_cast<int>(rval_type));
  }

  ctx.add_earlvar_to_scope(std::move(stmt->m_id), binding_type, false, expr_eval);
}

void eval_stmt(std::unique_ptr<Stmt> stmt, Ctx &ctx) {
  switch (stmt->stmt_type()) {
  case StmtType::Let: {
    eval_stmt_let(dynamic_cast<StmtLet *>(stmt.get()), ctx);
  } break;
  case StmtType::Mut: {
    assert(false && "unimplemented");
  } break;
  case StmtType::Def: {
    assert(false && "unimplemented");
  } break;
  case StmtType::Block: {
    assert(false && "unimplemented");
  } break;
  case StmtType::Stmt_Expr: {
    assert(false && "unimplemented");
  } break;
  default:
    assert(false && "eval_stmt: invalid statement");
  }
}

void interpret(Program &program) {
  Ctx ctx;

  for (size_t i = 0; i < program.m_stmts.size(); ++i) {
    eval_stmt(std::move(program.m_stmts.at(i)), ctx);
  }
}
