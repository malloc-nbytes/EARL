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

#include "ast.hpp"
#include "common.hpp"
#include "parser.hpp"

std::unique_ptr<Token> parse_expect(Lexer &lexer, TokenType expected) {
  std::unique_ptr<Token> tok(lexer.next());
  if (tok->m_type != expected) {
    assert(false && "parse_expect");
  }
  return tok;
}

std::unique_ptr<Token> parse_expect_keyword(Lexer &lexer, std::string expected) {
  std::unique_ptr<Token> tok(lexer.next());

  if (tok->m_type != TokenType::Keyword)
    assert(false && "parse_expect_keyword: not a keyword");
  if (tok->m_lexeme != expected)
    assert(false && "parse_expect_keyword: wrong keyword");

  return tok;
}

std::unique_ptr<Token> parse_expect_type(Lexer &lexer) {
  std::unique_ptr<Token> tok(lexer.next());

  if (tok->m_type != TokenType::Keyword)
    assert(false && "parse_expect_type: not a keyword");

  // TODO: verify that it is a type
  return tok;
}

std::unique_ptr<Expr> parse_primary_expr(Lexer &lexer) {
  Token *cur = lexer.next();

  switch (cur->type()) {
  case TokenType::Ident: {
    return std::make_unique<ExprIdent>(std::make_unique<Token>(*cur));
  } break;
  case TokenType::Intlit: {
    return std::make_unique<ExprIntLit>(std::make_unique<Token>(*cur));
  } break;
  case TokenType::Strlit: {
    return std::make_unique<ExprStrLit>(std::make_unique<Token>(*cur));
  } break;
  default:
    assert(false && "parse_primary_expr: invalid primary expression");
  }
}

std::unique_ptr<Expr> parse_multiplicative_expr(Lexer &lexer) {
  std::unique_ptr<Expr> lhs = parse_primary_expr(lexer);
  Token *cur = lexer.peek();
  while (cur && (cur->type() == TokenType::Asterisk
                 || cur->type() == TokenType::Forwardslash)) {
    std::unique_ptr<Token> op(lexer.next());
    std::unique_ptr<Expr> rhs = parse_primary_expr(lexer);
    lhs = std::make_unique<ExprBinary>(std::move(lhs), std::move(op), std::move(rhs));
    cur = lexer.peek();
  }
  return lhs;
}

std::unique_ptr<Expr> parse_additive_expr(Lexer &lexer) {
  std::unique_ptr<Expr> lhs = parse_multiplicative_expr(lexer);
  Token *cur = lexer.peek();
  while (cur && (cur->type() == TokenType::Plus
                 || cur->type() == TokenType::Minus)) {
    std::unique_ptr<Token> op(lexer.next());
    std::unique_ptr<Expr> rhs = parse_multiplicative_expr(lexer);
    lhs = std::make_unique<ExprBinary>(std::move(lhs), std::move(op), std::move(rhs));
    cur = lexer.peek();
  }
  return lhs;
}

std::unique_ptr<Expr> parse_equalitative_expr(Lexer &lexer) {
  std::unique_ptr<Expr> lhs = parse_additive_expr(lexer);
  Token *cur = lexer.peek();
  while (cur && (cur->type() == TokenType::Double_Equals
                 || cur->type() == TokenType::Greaterthan_Equals
                 || cur->type() == TokenType::Greaterthan
                 || cur->type() == TokenType::Lessthan_Equals
                 || cur->type() == TokenType::Lessthan
                 || cur->type() == TokenType::Bang_Equals)) {
    std::unique_ptr<Token> op(lexer.next());
    std::unique_ptr<Expr> rhs = parse_additive_expr(lexer);
    lhs = std::make_unique<ExprBinary>(std::move(lhs), std::move(op), std::move(rhs));
    cur = lexer.peek();
  }
  return lhs;
}

std::unique_ptr<Expr> parse_logical_expr(Lexer &lexer) {
  std::unique_ptr<Expr> lhs = parse_equalitative_expr(lexer);
  Token *cur = lexer.peek();
  while (cur && (cur->type() == TokenType::Double_Ampersand
                 || cur->type() == TokenType::Double_Pipe)) {
    std::unique_ptr<Token> op(lexer.next());
    std::unique_ptr<Expr> rhs = parse_equalitative_expr(lexer);
    lhs = std::make_unique<ExprBinary>(std::move(lhs), std::move(op), std::move(rhs));
    cur = lexer.peek();
  }
  return lhs;
}

std::unique_ptr<Expr> parse_expr(Lexer &lexer) {
  return parse_logical_expr(lexer);
}

std::unique_ptr<StmtLet> parse_stmt_let(Lexer &lexer) {
  (void)parse_expect_keyword(lexer, COMMON_KW_LET);
  std::unique_ptr<Token> id = parse_expect(lexer, TokenType::Ident);
  (void)parse_expect(lexer, TokenType::Colon);
  std::unique_ptr<Token> ty = parse_expect_type(lexer);
  (void)parse_expect(lexer, TokenType::Equals);
  std::unique_ptr<Expr> expr = parse_expr(lexer);
  (void)parse_expect(lexer, TokenType::Semicolon);
  return std::make_unique<StmtLet>(std::move(id), std::move(ty), std::move(expr));
}

std::unique_ptr<StmtDef> parse_stmt_def(Lexer &lexer) {
  assert(false && "unimplemented");
}

std::unique_ptr<Stmt> parse_stmt(Lexer &lexer) {
  Token *tok = lexer.next();
  switch (tok->type()) {
  case TokenType::Keyword: {
    if (tok->lexeme() == COMMON_KW_LET) {
      return parse_stmt_let(lexer);
    }
    else if (tok->lexeme() == COMMON_KW_DEF) {
      return parse_stmt_def(lexer);
    }
    else {
      assert(false && "parse_stmt: invalid keyword");
    }
  } break;
  case TokenType::Ident: {
  } break;
  default:
    assert(false && "parse_stmt: invalid statement");
  }
}

Program parse_program(Lexer &lexer) {
  std::vector<std::unique_ptr<Stmt>> stmts;

  while (lexer.peek()->type() != TokenType::Eof) {
    stmts.push_back(parse_stmt(lexer));
  }

  return Program(std::move(stmts));
}
