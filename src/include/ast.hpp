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
  Str,Literal,
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
  std::unique_ptr<Token> m_id;

public:
  ExprIdent(std::unique_ptr<Token> id);
  ExprType get_type() const override;
  ExprTermType get_term_type() const override;
  const Token &get_id() const;
};

class ExprIntLit : public ExprTerm {
public:
  // TODO
};

class ExprStrLit : public ExprTerm {
public:
  // TODO
};

class ExprBinary : public Expr {
public:
  // TODO
};

class ExprFuncCall : public Expr {
public:
  // TODO
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
  std::unique_ptr<Token> m_id;
  std::unique_ptr<Token> m_type;
  std::unique_ptr<Expr> m_expr;

public:
  StmtLet(std::unique_ptr<Token> id, std::unique_ptr<Token> type, std::unique_ptr<Expr> expr);

  StmtType stmt_type() const override;
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
  std::unique_ptr<Token> m_op;
  std::unique_ptr<Expr> m_right;

public:
  StmtMut(std::unique_ptr<Expr> left, std::unique_ptr<Token> op, std::unique_ptr<Expr> right);
  StmtType stmt_type() const override;
};

class StmtExpr : public Stmt {
  std::unique_ptr<Expr> m_expr;

public:
  StmtExpr(std::unique_ptr<Expr> expr);
  StmtType stmt_type() const override;
};

class Program {
  std::vector<std::unique_ptr<Stmt>> stmts;

public:
  // TODO
};

#endif // AST_H
