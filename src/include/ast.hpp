/** @file */

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
#include <optional>

#include "token.hpp"

/**
 * The grammar of EARL.
 */

/// The different types a statement can be.
enum class StmtType {
    Def,
    Let,
    Block,
    Mut,
    Stmt_Expr,
    If,
    Stmt_Return,
    Stmt_While,
    Stmt_For,
};

/// The different types an expression can be.
enum class ExprType {
    Term,
    Binary,
};

/// The different types a term can be.
enum class ExprTermType {
    Ident,
    Int_Literal,
    Str_Literal,
    Func_Call,
    List_Literal,
};

struct StmtBlock;

/// @brief Base class for an expression
struct Expr {
    virtual ~Expr() = default;

    /// @brief The get expression type
    /// @returns The type of the expression
    virtual ExprType get_type() const = 0;
};

/// @brief The Expression Term class
struct ExprTerm : public Expr {
    virtual ~ExprTerm() = default;

    /// @brief Get the Expression Term type
    /// @returns The type of the expression term
    virtual ExprTermType get_term_type() const = 0;
};

/// @brief The Expression Identifier class
struct ExprIdent : public ExprTerm {
    /// @brief The token of the identifier
    std::unique_ptr<Token> m_tok;

    ExprIdent(std::unique_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
};

/// @brief The Expression Integer Literal class
struct ExprIntLit : public ExprTerm {
    /// @brief The token of the integer literal
    std::unique_ptr<Token> m_tok;

    ExprIntLit(std::unique_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
};

/// @brief The Expression String Literal class
struct ExprStrLit : public ExprTerm {
    /// @brief The token of the string literal
    std::unique_ptr<Token> m_tok;

    ExprStrLit(std::unique_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
};

/// @brief The Expression Function Call class
struct ExprFuncCall : public ExprTerm {
    /// @brief The token of the function call
    std::unique_ptr<Token> m_id;

    /// @brief A vector of expressions that is passed
    /// to the function call
    std::vector<std::unique_ptr<Expr>> m_params;

    ExprFuncCall(std::unique_ptr<Token> id, std::vector<std::unique_ptr<Expr>> params);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
};

/// @brief The Expression List Literal class
struct ExprListLit : public ExprTerm {
    /// @brief The elements in the list
    std::vector<std::unique_ptr<Expr>> m_elems;

    ExprListLit(std::vector<std::unique_ptr<Expr>> elems);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
};

/// @brief The Expression Binary class
struct ExprBinary : public Expr {
    /// @brief The expression of the left hand side
    std::unique_ptr<Expr> m_lhs;

    /// @brief The token of the binary operator
    std::unique_ptr<Token> m_op;

    /// @brief The expression of the right hand side
    std::unique_ptr<Expr> m_rhs;

    ExprBinary(std::unique_ptr<Expr> lhs, std::unique_ptr<Token> op, std::unique_ptr<Expr> rhs);
    ExprType get_type() const override;
};

/// @brief The base Statement class
struct Stmt {
    virtual ~Stmt() = default;

    /// @brief Get the statement type
    /// @returns The type of the statement
    virtual StmtType stmt_type() const = 0;
};

/// @brief The Statement Definition Class
struct StmtDef : public Stmt {

    /// @brief The token of the function name
    std::unique_ptr<Token> m_id;

    /// @brief A vector of `<Identifer, Type>` of the statement definition
    std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>> m_args;

    /// @brief The token of the return type
    std::unique_ptr<Token> m_rettype;

    /// @brief The Statement Block of the Statement Definition
    std::unique_ptr<StmtBlock> m_block;

    uint32_t m_attrs;

    StmtDef(std::unique_ptr<Token> id,
            std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>> args,
            std::unique_ptr<Token> rettype,
            std::unique_ptr<StmtBlock> block,
            uint32_t attrs);

    StmtType stmt_type() const override;
};

/// @brief The Statement Let class
struct StmtLet : public Stmt {
    /// @brief The token of the identifer
    std::unique_ptr<Token> m_id;

    /// @brief The token of the Let binding
    std::unique_ptr<Token> m_type;

    /// @brief The expression of the Let Statement
    std::unique_ptr<Expr> m_expr;

    StmtLet(std::unique_ptr<Token> id, std::unique_ptr<Token> type, std::unique_ptr<Expr> expr);
    StmtType stmt_type() const override;
};

/// @brief The Statement Block class
struct StmtBlock : public Stmt {
    /// @brief A vector of statements that is in the block
    std::vector<std::unique_ptr<Stmt>> m_stmts;

    StmtBlock(std::vector<std::unique_ptr<Stmt>> stmts);
    void add_stmt(std::unique_ptr<Stmt> stmt);
    StmtType stmt_type() const override;
};

/// @brief The Statement Mutation class
struct StmtMut : public Stmt {
    /// @brief The expression of the left hand side
    std::unique_ptr<Expr> m_left;

    /// @brief The expression of the right hand side
    std::unique_ptr<Expr> m_right;

    StmtMut(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);
    StmtType stmt_type() const override;
};

/// @brief The Statement Expression class
struct StmtExpr : public Stmt {
    /// @brief The expression to be evaluated
    std::unique_ptr<Expr> m_expr;

    StmtExpr(std::unique_ptr<Expr> expr);
    StmtType stmt_type() const override;
};

/// @brief The Statement If class
struct StmtIf : public Stmt {
    /// @brief The expression that the `if` evaluates
    std::unique_ptr<Expr> m_expr;

    /// @brief The `block` of the statement.
    std::unique_ptr<StmtBlock> m_block;

    /// @brief The `block` of the else part of the `if` statement.
    std::optional<std::unique_ptr<StmtBlock>> m_else;

    StmtIf(std::unique_ptr<Expr> expr,
           std::unique_ptr<StmtBlock> block,
           std::optional<std::unique_ptr<StmtBlock>> else_);

    StmtType stmt_type() const override;
};

/// @brief The Statement Return class
struct StmtReturn : public Stmt {
    /// @brief The expression that the Statement Return is returning
    std::unique_ptr<Expr> m_expr;

    StmtReturn(std::unique_ptr<Expr> expr);
    StmtType stmt_type() const override;
};

/// @brief The Statement While class
struct StmtWhile : public Stmt {
    /// @brief The expression to loop while it is true
    std::unique_ptr<Expr> m_expr;

    /// @brief The block of the while loop to loop
    std::unique_ptr<StmtBlock> m_block;

    StmtWhile(std::unique_ptr<Expr> expr, std::unique_ptr<StmtBlock> block);
    StmtType stmt_type() const override;
};

/// @brief The Statement For class
struct StmtFor : public Stmt {
    /// @brief The identifier of the enumerator variable
    std::unique_ptr<Token> m_enumerator;

    /// @brief The starting expression
    std::unique_ptr<Expr> m_start;

    /// @brief The starting expression
    std::unique_ptr<Expr> m_end;

    /// @brief The block for the loop to execute
    std::unique_ptr<StmtBlock> m_block;

    StmtFor(std::unique_ptr<Token> enumerator,
            std::unique_ptr<Expr> start,
            std::unique_ptr<Expr> end,
            std::unique_ptr<StmtBlock> block);

    StmtType stmt_type() const override;
};

/// @brief The Program class. It is the starting point
/// of the whole program.
struct Program {
    /// @brief A vector of statement to evaluate
    std::vector<std::unique_ptr<Stmt>> m_stmts;

    Program(std::vector<std::unique_ptr<Stmt>> stmts);
};

#endif // AST_H
