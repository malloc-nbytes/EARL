#include <cassert>
#include <memory>

#include "ast.hpp"

/*** TERM EXPRESSIONS ***/

ExprIdent::ExprIdent(std::unique_ptr<Token> id) : m_id(std::move(id)) {}

ExprType ExprIdent::get_type() const {
  return ExprType::Term;
}

ExprTermType ExprIdent::get_term_type() const {
  return ExprTermType::Ident;
}

const Token &ExprIdent::get_id() const {
  assert(false && "unimplemented");
}

/*** STATEMENTS ***/

StmtDef::StmtDef(std::unique_ptr<Token> id,
                 std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>> args,
                 std::unique_ptr<Token> rettype,
                 std::unique_ptr<StmtBlock> block) :
    m_id(std::move(id)), m_args(std::move(args)),
    m_rettype(std::move(rettype)), m_block(std::move(block)) {}

StmtType StmtDef::stmt_type() const {
  return StmtType::Def;
}

StmtLet::StmtLet(std::unique_ptr<Token> id,
                 std::unique_ptr<Token> type,
                 std::unique_ptr<Expr> expr)
  : m_id(std::move(id)), m_type(std::move(type)), m_expr(std::move(expr)) {}

StmtType StmtLet::stmt_type() const {
  return StmtType::Let;
}

StmtBlock::StmtBlock(std::vector<std::unique_ptr<Stmt>> stmts) : m_stmts(std::move(stmts)) {}

void add_stmt(std::unique_ptr<Stmt> stmt) {
  assert(false && "unimplemented");
}

const std::vector<std::unique_ptr<Stmt>> &StmtBlock::get_stmts() const {
  assert(false && "unimplemented");
}

StmtType StmtBlock::stmt_type() const {
  return StmtType::Block;
}

StmtMut::StmtMut(std::unique_ptr<Expr> left,
                 std::unique_ptr<Token> op,
                 std::unique_ptr<Expr> right)
  : m_left(std::move(left)), m_op(std::move(op)), m_right(std::move(right)) {}

StmtType StmtMut::stmt_type() const {
  return StmtType::Mut;
}

StmtExpr::StmtExpr(std::unique_ptr<Expr> expr) : m_expr(std::move(expr)) {}

StmtType StmtExpr::stmt_type() const {
  return StmtType::Stmt_Expr;
}
