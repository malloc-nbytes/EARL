#include <cassert>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "interpreter.hpp"
#include "token.hpp"
#include "ast.hpp"

struct EarlVar {
  std::unique_ptr<Token> m_id;
  std::unique_ptr<Token> m_type;
  void *m_value;
};

struct Ctx {
  std::vector<std::unordered_map<std::string, EarlVar>> m_scope;

  Ctx() {
    m_scope.emplace_back();
  }

  void add_var_to_scope(std::unique_ptr<Token> id, std::unique_ptr<Token> type, void *value) {
    m_scope.back()[id->lexeme()] = {std::move(id), std::move(type), value};
  }
};

void push_scope(Ctx &ctx) {
  ctx.m_scope.emplace_back();
}

static void *eval_expr(std::unique_ptr<Expr> expr, Ctx &ctx)
{
  (void)expr;
  (void)ctx;
  return NULL;
  // switch (expr->type) {
  // case EXPR_TYPE_TERM:
  //   UNIMPLEMENTED("eval_expr: EXPR_TYPE_TERM");
  // case EXPR_TYPE_BINARY:
  //   UNIMPLEMENTED("eval_expr: EXPR_TYPE_BINARY");
  // case EXPR_TYPE_FUNCCALL:
  //   UNIMPLEMENTED("eval_expr: EXPR_TYPE_FUNCCALL");
  // default:
  //   NOTIFY_ERRARGS(NOTIFY_ERR_SYNTAX, "eval_expr: unknown expr type: %d", expr->type);
  // }
}

static void eval_stmt_let(std::unique_ptr<StmtLet> stmt, Ctx &ctx)
{
  (void)stmt;
  (void)ctx;
}

static void eval_stmt(std::unique_ptr<Stmt> stmt, Ctx &ctx)
{
  (void)stmt;
  (void)ctx;
  switch (stmt->stmt_type()) {
  case StmtType::Let: {
    // StmtLet *stmt_let = dynamic_cast<StmtLet *>(stmt.get());
    // eval_stmt_let(stmt_let, ctx);
    break;
  }
  default:
    assert(false && "eval_stmt: unknown stmt type");
  }
}

void interpret(Program &program)
{
  Ctx ctx;

  for (size_t i = 0; i < program.stmts_len(); ++i) {
    eval_stmt(std::move(program.get_stmt(i)), ctx);
  }
}
