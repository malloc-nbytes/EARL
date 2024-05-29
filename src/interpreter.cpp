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

struct ExprEvalResult {
  // The actual evaluated result.
  std::any m_expr_value;

  // What kind of term did we encounter?
  // Integer? Identifier? etc...
  ExprTermType m_expr_term_type;
};

static expr_term_type_to_earlty_type(ExprTermType type, Ctx &ctx) {

}

ExprEvalResult eval_expr_term(ExprTerm *expr, Ctx &ctx) {
  (void)expr;
  (void)ctx;
  return ExprEvalResult {};
}

ExprEvalResult eval_expr_bin(ExprBinary *expr, Ctx &ctx) {
  (void)expr;
  (void)ctx;
  return ExprEvalResult {};
}

ExprEvalResult eval_expr(Expr *expr, Ctx &ctx) {
  (void)expr;
  (void)ctx;
  return ExprEvalResult {};
}

void eval_stmt_let(StmtLet *stmt, Ctx &ctx) {
  const std::string &id = stmt->m_id->lexeme();
  if (ctx.earlvar_in_scope(id)) {
    ERR_WARGS(ErrType::Redeclared, "variable `%s` is already defined", id.c_str());
  }

  EarlTy::Type binding_type = EarlTy::of_str(stmt->m_type->lexeme());
  ExprEvalResult expr_eval = eval_expr(stmt->m_expr.get(), ctx);
  // EarlTy::Type rval_type = EarlTy

  switch (expr_eval.m_expr_term_type) {
  case ExprTermType::Ident: {
    assert(false && "unimplemented");
  } break;
  case ExprTermType::Int_Literal: {
    assert(false && "unimplemented");
  } break;
  case ExprTermType::Str_Literal: {
    assert(false && "unimplemented");
  } break;
  default:
    ERR_WARGS(ErrType::Fatal,
              "expression evaluation return type (%d) is not a valid return tyep",
              static_cast<int>(expr_eval.m_expr_term_type));
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
