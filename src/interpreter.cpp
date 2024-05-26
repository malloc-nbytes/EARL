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

  EarlVar &get_var(const std::string &id) {
    for (auto it = m_scope.rbegin(); it != m_scope.rend(); ++it) {
      if (it->find(id) != it->end()) {
        return it->at(id);
      }
    }
    assert(false && "get_var: variable not found");
  }
};

static std::any eval_expr(Expr *, Ctx &);

void debug_dump_scope(Ctx &ctx) {
  for (auto &scope : ctx.m_scope) {
    for (auto &var : scope) {
      switch (var.second.m_type.get()->type()) {
        case TokenType::Keyword: // TODO: add type checking
          std::cout << var.first << " = " << std::any_cast<int>(var.second.m_value) << std::endl;
          break;
        default:
          std::cerr << "error: unknown type" << std::endl;
          break;
      }
    }
  }
}

void scope_pop(Ctx &ctx) {
  ctx.m_scope.pop_back();
}

void scope_push(Ctx &ctx) {
  ctx.m_scope.emplace_back();
}

static std::any eval_expr_term(ExprTerm *expr, Ctx &ctx) {
  switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
      ExprIdent *expr_ident = dynamic_cast<ExprIdent *>(expr);
      if (!ctx.has_var(expr_ident->tok().lexeme())) {
        std::cerr << "error: variable '" << expr_ident->tok().lexeme() << "' not declared" << std::endl;
        return nullptr;
      }
      return ctx.get_var(expr_ident->tok().lexeme()).m_value;
    } break;
    case ExprTermType::Int_Literal: {
      ExprIntLit *expr_intlit = dynamic_cast<ExprIntLit *>(expr);
      return std::stoi(expr_intlit->tok().lexeme());
    } break;
    case ExprTermType::Str_Literal: {
      assert(false && "eval_expr_term: string literals not implemented");
    } break;
    default:
      assert(false && "eval_expr_term: unknown term type");
  }
}

static std::any eval_expr_binary(ExprBinary *expr, Ctx &ctx)
{
  std::any lhs = eval_expr(&expr->lhs(), ctx);
  std::any rhs = eval_expr(&expr->rhs(), ctx);

  // TODO: type checking and different types
  switch (expr->op().type()) {
  case TokenType::Plus:
    return std::any_cast<int>(lhs) + std::any_cast<int>(rhs);
  case TokenType::Minus:
    return std::any_cast<int>(lhs) - std::any_cast<int>(rhs);
  case TokenType::Asterisk:
    return std::any_cast<int>(lhs) * std::any_cast<int>(rhs);
  case TokenType::Forwardslash:
    return std::any_cast<int>(lhs) / std::any_cast<int>(rhs);
  default:
    assert(false && "eval_expr_binary: unknown operator");
  }
}

static std::any eval_expr(Expr *expr, Ctx &ctx)
{
  switch (expr->get_type()) {
  case ExprType::Term: {
    return eval_expr_term(dynamic_cast<ExprTerm *>(expr), ctx);
  } break;
  case ExprType::Binary: {
    return eval_expr_binary(dynamic_cast<ExprBinary *>(expr), ctx);
  }
  case ExprType::Func_Call:
    assert(false && "todo");
  default:
    assert(false && "eval_expr: unknown expression");
  }
}

static void eval_stmt_let(StmtLet *stmt, Ctx &ctx)
{
  if (ctx.has_var(stmt->id().lexeme())) {
    std::cerr << "error: variable '" << stmt->id().lexeme() << "' already declared" << std::endl;
    return;
  }
  std::any value = eval_expr(&stmt->expr(), ctx);
  ctx.add_var(std::move(stmt->m_id), std::move(stmt->m_type), value);
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
  Ctx ctx;

  for (size_t i = 0; i < program.stmts_len(); ++i) {
    eval_stmt(std::move(program.get_stmt(i)), ctx);
  }

  debug_dump_scope(ctx);
}
