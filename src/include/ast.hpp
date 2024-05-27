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

#ifndef AST_H
#define AST_H

#include <vector>
#include <memory>

#include "token.hpp"

enum class StmtType {
  Def,
  Let,
  Block,
  Mut,
  Stmt_Expr,
};

enum class ExprType {
  Term,
  Binary,
  Func_Call
};

enum class ExprTermType {
  Ident,
  Int_Literal,
  Str_Literal,
};

class StmtBlock;

class Expr {
public:
  virtual ~Expr() = default;
  virtual ExprType get_type() const = 0;
};

class ExprTerm : public Expr {
public:
  virtual ~ExprTerm() = default;
  virtual ExprTermType get_term_type() const = 0;
};

class ExprIdent : public ExprTerm {
  std::unique_ptr<Token> m_tok;

public:
  ExprIdent(std::unique_ptr<Token> tok);
  ExprType get_type() const override;
  ExprTermType get_term_type() const override;
  const Token &tok() const;
};

class ExprIntLit : public ExprTerm {
  std::unique_ptr<Token> m_tok;

public:
  ExprIntLit(std::unique_ptr<Token> tok);
  ExprType get_type() const override;
  ExprTermType get_term_type() const override;
  const Token &tok() const;
};

class ExprStrLit : public ExprTerm {
public:
  std::unique_ptr<Token> m_tok;

public:
  ExprStrLit(std::unique_ptr<Token> tok);
  ExprType get_type() const override;
  ExprTermType get_term_type() const override;
  const Token &get_tok() const;
};

class ExprBinary : public Expr {
  std::unique_ptr<Expr> m_lhs;
  std::unique_ptr<Token> m_op;
  std::unique_ptr<Expr> m_rhs;

public:
  ExprBinary(std::unique_ptr<Expr> lhs, std::unique_ptr<Token> op, std::unique_ptr<Expr> rhs);
  ExprType get_type() const override;
  Expr &lhs();
  const Token &op() const;
  Expr &rhs();
};

class ExprFuncCall : public Expr {
  std::unique_ptr<Token> m_id;
  std::vector<std::unique_ptr<Token>> m_params;

public:
  ExprFuncCall(std::unique_ptr<Token> id, std::vector<std::unique_ptr<Token>> params);
  ExprType get_type() const override;
};

class Stmt {
public:
  virtual ~Stmt() = default;
  virtual StmtType stmt_type() const = 0;
};

class StmtDef : public Stmt {

  // Function name
  std::unique_ptr<Token> m_id;

  // id * type
  std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>> m_args;
  std::unique_ptr<Token> m_rettype;
  std::unique_ptr<StmtBlock> m_block;

public:
  StmtDef(std::unique_ptr<Token> id,
          std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>> args,
          std::unique_ptr<Token> rettype,
          std::unique_ptr<StmtBlock> block);

  StmtType stmt_type() const override;
};

class StmtLet : public Stmt {

public:
  std::unique_ptr<Token> m_id;
  std::unique_ptr<Token> m_type;
  std::unique_ptr<Expr> m_expr;

  StmtLet(std::unique_ptr<Token> id, std::unique_ptr<Token> type, std::unique_ptr<Expr> expr);
  StmtType stmt_type() const override;
  const Token &id() const;
  const Token &type() const;
  Expr &expr();
};

class StmtBlock : public Stmt {
  std::vector<std::unique_ptr<Stmt>> m_stmts;

public:
  StmtBlock(std::vector<std::unique_ptr<Stmt>> stmts);
  void add_stmt(std::unique_ptr<Stmt> stmt);
  const std::vector<std::unique_ptr<Stmt>> &get_stmts() const;
  StmtType stmt_type() const override;
};

class StmtMut : public Stmt {
  std::unique_ptr<Expr> m_left;
  std::unique_ptr<Expr> m_right;

public:
  StmtMut(std::unique_ptr<Expr> left, /*std::unique_ptr<Token> op,*/ std::unique_ptr<Expr> right);
  StmtType stmt_type() const override;
  Expr &left();
  Expr &right();
};

class StmtExpr : public Stmt {
  std::unique_ptr<Expr> m_expr;

public:
  StmtExpr(std::unique_ptr<Expr> expr);
  StmtType stmt_type() const override;
};

class Program {
  std::vector<std::unique_ptr<Stmt>> m_stmts;

public:
  Program(std::vector<std::unique_ptr<Stmt>> stmts);
  std::unique_ptr<Stmt> &get_stmt(size_t i);
  size_t stmts_len(void) const;
};

#endif // AST_H
