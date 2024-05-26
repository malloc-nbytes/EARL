#include <cassert>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <any>

#include "interpreter.hpp"
#include "token.hpp"
#include "ast.hpp"

struct EarlVar {
  std::unique_ptr<Token> m_id;
  std::unique_ptr<Token> m_type;
  std::any m_value;

  uint32_t m_refcount;

  EarlVar(std::unique_ptr<Token> id, std::unique_ptr<Token> type, std::any value = nullptr, uint32_t refcount = 1)
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

  void add_var(std::unique_ptr<Token> id, std::unique_ptr<Token> type, std::any value = nullptr) {
    std::string name = id->lexeme();
    m_scope.back().emplace(name, EarlVar(std::move(id), std::move(type), std::move(value)));
  }

  bool has_var(const std::string &id) const {
    for (auto it = m_scope.rbegin(); it != m_scope.rend(); ++it) {
      if (it->find(id) != it->end()) {
        return true;
      }
    }
    return false;
  }
};

void scope_pop(Ctx &ctx) {
  ctx.m_scope.pop_back();
}

void scope_push(Ctx &ctx) {
  ctx.m_scope.emplace_back();
}

static std::any eval_expr(const Expr *expr, Ctx &ctx)
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
  if (ctx.has_var(stmt->id().lexeme())) {
    std::cerr << "error: variable '" << stmt->id().lexeme() << "' already declared" << std::endl;
    return;
  }
  // std::any value = eval_expr(&stmt->expr(), ctx);
  ctx.add_var(std::move(stmt->m_id), std::move(stmt->m_type));
}

static void eval_stmt(std::unique_ptr<Stmt> stmt, Ctx &ctx)
{
  (void)stmt;
  (void)ctx;
  switch (stmt->stmt_type()) {
  case StmtType::Let: {
    if (auto stmt_let = dynamic_cast<StmtLet *>(stmt.get())) {
      eval_stmt_let(stmt_let, ctx);
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
  (void)eval_expr;

  Ctx ctx;

  for (size_t i = 0; i < program.stmts_len(); ++i) {
    eval_stmt(std::move(program.get_stmt(i)), ctx);
  }
}
