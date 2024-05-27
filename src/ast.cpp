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

#include <cassert>
#include <memory>

#include "ast.hpp"

/*** PROGRAM ***/

Program::Program(std::vector<std::unique_ptr<Stmt>> stmts)
  : m_stmts(std::move(stmts)) {}

std::unique_ptr<Stmt> &Program::get_stmt(size_t i) {
  return m_stmts.at(i);
}

size_t Program::stmts_len(void) const {
  return m_stmts.size();
}

/*** EXPRESSIONS ***/

ExprFuncCall::ExprFuncCall(std::unique_ptr<Token> id,
                           std::vector<std::unique_ptr<Token>> params)
  : m_id(std::move(id)), m_params(std::move(params)) {}

ExprType ExprFuncCall::get_type() const {
  return ExprType::Func_Call;
}

ExprBinary::ExprBinary(std::unique_ptr<Expr> lhs, std::unique_ptr<Token> op, std::unique_ptr<Expr> rhs)
  : m_lhs(std::move(lhs)), m_op(std::move(op)), m_rhs(std::move(rhs)) {}

ExprType ExprBinary::get_type() const {
  return ExprType::Binary;
}

Expr &ExprBinary::lhs() {
  return *m_lhs;
}

const Token &ExprBinary::op() const {
  return *m_op;
}

Expr &ExprBinary::rhs() {
  return *m_rhs;
}

/*** TERM EXPRESSIONS ***/

ExprStrLit::ExprStrLit(std::unique_ptr<Token> tok) : m_tok(std::move(tok)) {}

ExprType ExprStrLit::get_type() const {
  return ExprType::Term;
}

ExprTermType ExprStrLit::get_term_type() const {
  return ExprTermType::Str_Literal;
}

const Token &ExprStrLit::get_tok() const {
  assert(false && "unimplemented");
}

ExprIntLit::ExprIntLit(std::unique_ptr<Token> tok) : m_tok(std::move(tok)) {}

ExprType ExprIntLit::get_type() const {
  return ExprType::Term;
}

ExprTermType ExprIntLit::get_term_type() const {
  return ExprTermType::Int_Literal;
}

const Token &ExprIntLit::tok() const {
  return *m_tok;
}

ExprIdent::ExprIdent(std::unique_ptr<Token> tok) : m_tok(std::move(tok)) {}

ExprType ExprIdent::get_type() const {
  return ExprType::Term;
}

ExprTermType ExprIdent::get_term_type() const {
  return ExprTermType::Ident;
}

const Token &ExprIdent::tok() const {
  return *m_tok;
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

const Token &StmtLet::id() const {
  return *m_id;
}

const Token &StmtLet::type() const {
  return *m_type;
}

Expr &StmtLet::expr() {
  return *m_expr;
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
                 /*std::unique_ptr<Token> op,*/
                 std::unique_ptr<Expr> right)
  : m_left(std::move(left)), /*m_op(std::move(op)),*/ m_right(std::move(right)) {}

StmtType StmtMut::stmt_type() const {
  return StmtType::Mut;
}

Expr &StmtMut::left() {
  return *m_left;
}

Expr &StmtMut::right() {
  return *m_right;
}

StmtExpr::StmtExpr(std::unique_ptr<Expr> expr) : m_expr(std::move(expr)) {}

StmtType StmtExpr::stmt_type() const {
  return StmtType::Stmt_Expr;
}
