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

  uint32_t m_refcount;

  EarlVar(std::unique_ptr<Token> id, std::unique_ptr<Token> type, void *value = nullptr, uint32_t refcount = 1)
    : m_id(std::move(id)), m_type(std::move(type)), m_value(value), m_refcount(refcount) {}
};

struct Ctx {
  std::vector<std::unordered_map<std::string, EarlVar>> m_scope;

  Ctx() {
    m_scope.emplace_back();
  }

  ~Ctx() {
    m_scope.clear();
  }
};

void scope_add_var(std::unique_ptr<Token> id, std::unique_ptr<Token> type, Ctx &ctx) {
  ctx.m_scope.back().emplace(id->lexeme(), EarlVar(std::move(id), std::move(type)));
}

void scope_pop(Ctx &ctx) {
  ctx.m_scope.pop_back();
}

void scope_push(Ctx &ctx) {
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

static void eval_stmt_let(StmtLet *stmt, Ctx &ctx)
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
    if (auto stmtLet = dynamic_cast<StmtLet *>(stmt.get())) {
      eval_stmt_let(stmtLet, ctx);
    } else {
      assert(false && "eval_stmt: invalid stmt type");
    }
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
