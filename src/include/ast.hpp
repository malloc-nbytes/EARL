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

#include <variant>
#include <vector>
#include <memory>
#include <optional>

#include "token.hpp"

/**
 * The grammar of EARL.
 */

/// The different types a statement can be.
enum class StmtType {
    Def=0,
    Let,
    Block,
    Mut,
    Stmt_Expr,
    If,
    Return,
    Break,
    While,
    Loop,
    For,
    Foreach,
    Import,
    Mod,
    Class,
    Match,
    Enum,
    Continue,
    Bash_Literal,
    Info,
    Pipe,
    Multiline_Bash,
    Use,
    Exec,
    With,
};

/// The different types an expression can be.
enum class ExprType {
    Term,
    Binary,
    Unary,
};

/// The different types a term can be.
enum class ExprTermType {
    Ident,
    Int_Literal,
    Str_Literal,
    Char_Literal,
    Func_Call,
    List_Literal,
    Range,
    Slice,
    Get,
    Mod_Access,
    Array_Access,
    Bool,
    None,
    Closure,
    Tuple,
    Float_Literal,
    Dict,
    FStr,
    Power,
    Case,
    Predicate,
};

struct StmtDef;
struct StmtBlock;
struct ExprFuncCall;

struct __Type {
    std::shared_ptr<Token> m_main_ty;
    std::optional<std::shared_ptr<Token>> m_sub_ty;

    __Type(std::shared_ptr<Token> main_ty, std::optional<std::shared_ptr<Token>> sub_ty);
};

/// @brief Base class for an expression
struct Expr {
    virtual ~Expr() = default;

    /// @brief The get expression type
    /// @returns The type of the expression
    virtual ExprType get_type() const = 0;
    virtual size_t get_lineno() const = 0;
};

/// @brief The Expression Term class
struct ExprTerm : public Expr {
    virtual ~ExprTerm() = default;

    /// @brief Get the Expression Term type
    /// @returns The type of the expression term
    virtual ExprTermType get_term_type() const = 0;
};

struct ExprTuple : public ExprTerm {
    std::vector<std::unique_ptr<Expr>> m_exprs;
    std::shared_ptr<Token> m_tok;

    ExprTuple(std::vector<std::unique_ptr<Expr>> exprs, std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprDict : public ExprTerm {
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> m_values;
    std::shared_ptr<Token> m_tok;

    ExprDict(std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> values,
             std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprArrayAccess : public ExprTerm {
    std::unique_ptr<Expr> m_left;
    std::unique_ptr<Expr> m_expr;
    std::shared_ptr<Token> m_tok;

    ExprArrayAccess(std::unique_ptr<Expr> left, std::unique_ptr<Expr> expr, std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression Identifier class
struct ExprIdent : public ExprTerm {
    /// @brief The token of the identifier
    std::shared_ptr<Token> m_tok;

    ExprIdent(std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprGet : public ExprTerm {
    std::unique_ptr<Expr> m_left;
    std::variant<std::unique_ptr<ExprIdent>, std::unique_ptr<ExprFuncCall>> m_right;
    std::shared_ptr<Token> m_tok;

    ExprGet(std::unique_ptr<Expr> left,
            std::variant<std::unique_ptr<ExprIdent>, std::unique_ptr<ExprFuncCall>> right,
            std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprModAccess : public ExprTerm {
    std::unique_ptr<ExprIdent> m_expr_ident;
    std::variant<std::unique_ptr<ExprIdent>, std::unique_ptr<ExprFuncCall>> m_right;
    std::shared_ptr<Token> m_tok;

    ExprModAccess(std::unique_ptr<ExprIdent> expr_ident,
                  std::variant<std::unique_ptr<ExprIdent>, std::unique_ptr<ExprFuncCall>> right,
                  std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprCase : public ExprTerm {
    struct Case {
        std::unique_ptr<Expr> m_lhs;
        std::unique_ptr<Expr> m_rhs;
        Case(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);
    };

    std::shared_ptr<Token> m_errtok;
    std::unique_ptr<Expr> m_expr;
    std::vector<std::unique_ptr<Case>> m_cases;

    ExprCase(std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<Case>> cases);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprPredicate : public ExprTerm {
    std::shared_ptr<Token> m_op;
    std::unique_ptr<Expr> m_rhs;

    ExprPredicate(std::shared_ptr<Token> op, std::unique_ptr<Expr> rhs);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression Integer Literal class
struct ExprIntLit : public ExprTerm {
    /// @brief The token of the integer literal
    std::shared_ptr<Token> m_tok;

    ExprIntLit(std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression Float Literal class
struct ExprFloatLit : public ExprTerm {
    /// @brief The token of the integer literal
    std::shared_ptr<Token> m_tok;

    ExprFloatLit(std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression String Literal class
struct ExprStrLit : public ExprTerm {
    /// @brief The token of the string literal
    std::shared_ptr<Token> m_tok;

    ExprStrLit(std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprFStr : public ExprTerm {
    std::shared_ptr<Token> m_tok;

    ExprFStr(std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprCharLit : public ExprTerm {
    std::shared_ptr<Token> m_tok;

    ExprCharLit(std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression Bool class
struct ExprBool : public ExprTerm {
    std::shared_ptr<Token> m_tok;
    bool m_value;

    ExprBool(std::shared_ptr<Token> tok, bool value);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression None class
struct ExprNone : public ExprTerm {
    std::shared_ptr<Token> m_tok;

    ExprNone(std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprClosure : public ExprTerm {
    std::vector<std::pair<std::shared_ptr<Token>, uint32_t>> m_args;
    std::unique_ptr<StmtBlock> m_block;
    std::shared_ptr<Token> m_tok;

    ExprClosure(std::vector<std::pair<std::shared_ptr<Token>, uint32_t>> args,
                std::unique_ptr<StmtBlock> block,
                std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression Function Call class
struct ExprFuncCall : public ExprTerm {
    // Needs to be an expr to handle either
    // ident or something like arr[0]().
    std::unique_ptr<Expr> m_left;

    /// @brief A vector of expressions that is passed
    /// to the function call
    std::vector<std::unique_ptr<Expr>> m_params;

    std::shared_ptr<Token> m_tok;

    ExprFuncCall(std::unique_ptr<Expr> id, std::vector<std::unique_ptr<Expr>> params, std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression List Literal class
struct ExprListLit : public ExprTerm {
    /// @brief The elements in the list
    std::vector<std::unique_ptr<Expr>> m_elems;
    std::shared_ptr<Token> m_tok;

    ExprListLit(std::vector<std::unique_ptr<Expr>> elems, std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprRange : public ExprTerm {
    std::unique_ptr<Expr> m_start;
    std::unique_ptr<Expr> m_end;
    bool m_inclusive;
    std::shared_ptr<Token> m_tok;

    ExprRange(std::unique_ptr<Expr> start, std::unique_ptr<Expr> end, bool inclusive, std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprSlice : public ExprTerm {
    std::optional<std::unique_ptr<Expr>> m_start;
    std::optional<std::unique_ptr<Expr>> m_end;
    std::shared_ptr<Token> m_tok;

    ExprSlice(std::optional<std::unique_ptr<Expr>> start, std::optional<std::unique_ptr<Expr>> end, std::shared_ptr<Token> tok);
    ExprType get_type() const override;
    ExprTermType get_term_type() const override;
    size_t get_lineno() const override;
};

struct ExprUnary : public Expr {
    std::shared_ptr<Token> m_op;
    std::unique_ptr<Expr> m_expr;

    ExprUnary(std::shared_ptr<Token> op, std::unique_ptr<Expr> expr);
    ExprType get_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Expression Binary class
struct ExprBinary : public Expr {
    /// @brief The expression of the left hand side
    std::unique_ptr<Expr> m_lhs;

    /// @brief The token of the binary operator
    std::shared_ptr<Token> m_op;

    /// @brief The expression of the right hand side
    std::unique_ptr<Expr> m_rhs;

    ExprBinary(std::unique_ptr<Expr> lhs, std::shared_ptr<Token> op, std::unique_ptr<Expr> rhs);
    ExprType get_type() const override;
    size_t get_lineno() const override;
};

/// @brief The base Statement class
struct Stmt {
    virtual ~Stmt() = default;

    /// @brief Get the statement type
    /// @returns The type of the statement
    virtual StmtType stmt_type() const = 0;
    virtual size_t get_lineno() const = 0;

    bool m_evald = false;
};

struct StmtInfo : public Stmt {
    std::string m_info;
    StmtInfo(std::string info);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtBashLiteral : public Stmt {
    std::unique_ptr<Expr> m_expr;
    StmtBashLiteral(std::unique_ptr<Expr> expr);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtExec : public Stmt {
    std::shared_ptr<Token> m_ident;
    std::shared_ptr<Token> m_tok;

    StmtExec(std::shared_ptr<Token> ident, std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtMultilineBash : public Stmt {
    std::shared_ptr<Token> m_sh;
    StmtMultilineBash(std::shared_ptr<Token> sh);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement Definition Class
struct StmtDef : public Stmt {

    /// @brief The token of the function name
    std::shared_ptr<Token> m_id;

    // the type below is: pair(pair(id, type), attr)
    std::vector<std::pair<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>, uint32_t>> m_args;

    std::optional<std::shared_ptr<__Type>> m_ty;

    /// @brief The Statement Block of the Statement Definition
    std::unique_ptr<StmtBlock> m_block;

    uint32_t m_attrs;
    std::vector<std::string> m_info;

    std::shared_ptr<Token> m_tok;

    StmtDef(std::shared_ptr<Token> id,
            std::vector<std::pair<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>, uint32_t>> args,
            std::optional<std::shared_ptr<__Type>> ty,
            std::unique_ptr<StmtBlock> block,
            uint32_t attrs,
            std::vector<std::string> info,
            std::shared_ptr<Token> tok);

    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement Let class
struct StmtLet : public Stmt {
    /// @brief The token of the identifer
    std::vector<std::shared_ptr<Token>> m_ids;

    std::vector<std::shared_ptr<__Type>> m_tys;

    /// @brief The expression of the Let Statement
    std::unique_ptr<Expr> m_expr;

    uint32_t m_attrs;

    std::vector<std::string> m_info;

    std::shared_ptr<Token> m_tok;

    StmtLet(std::vector<std::shared_ptr<Token>> ids,
            std::vector<std::shared_ptr<__Type>> tys,
            std::unique_ptr<Expr> expr,
            uint32_t attrs,
            std::vector<std::string> info,
            std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtWith : public Stmt {
    std::vector<std::shared_ptr<Token>> m_ids;
    std::vector<std::unique_ptr<Expr>> m_exprs;
    std::unique_ptr<Stmt> m_stmt;

    std::shared_ptr<Token> m_tok;

    StmtWith(std::vector<std::shared_ptr<Token>> ids,
             std::vector<std::unique_ptr<Expr>> exprs,
             std::unique_ptr<Stmt> stmt,
             std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtPipe : public Stmt {
    std::variant<std::unique_ptr<StmtBashLiteral>, std::unique_ptr<StmtExec>> m_bash;
    std::variant<std::shared_ptr<Token>, std::unique_ptr<Expr>> m_to;
    uint32_t m_attrs;
    std::vector<std::string> m_info;

    StmtPipe(std::variant<std::unique_ptr<StmtBashLiteral>, std::unique_ptr<StmtExec>> bash,
             std::variant<std::shared_ptr<Token>, std::unique_ptr<Expr>> to,
             uint32_t attrs,
             std::vector<std::string> info);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement Block class
struct StmtBlock : public Stmt {
    /// @brief A vector of statements that is in the block
    std::vector<std::unique_ptr<Stmt>> m_stmts;
    std::shared_ptr<Token> m_tok;

    StmtBlock(std::vector<std::unique_ptr<Stmt>> stmts, std::shared_ptr<Token> tok);
    void add_stmt(std::unique_ptr<Stmt> stmt);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement Mutation class
struct StmtMut : public Stmt {
    /// @brief The expression of the left hand side
    std::unique_ptr<Expr> m_left;

    /// @brief The expression of the right hand side
    std::unique_ptr<Expr> m_right;

    std::shared_ptr<Token> m_equals;

    StmtMut(std::unique_ptr<Expr> left,
            std::unique_ptr<Expr> right,
            std::shared_ptr<Token> m_equals);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement Expression class
struct StmtExpr : public Stmt {
    /// @brief The expression to be evaluated
    std::unique_ptr<Expr> m_expr;

    StmtExpr(std::unique_ptr<Expr> expr);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement If class
struct StmtIf : public Stmt {
    /// @brief The expression that the `if` evaluates
    std::unique_ptr<Expr> m_expr;

    /// @brief The `block` of the statement.
    std::unique_ptr<StmtBlock> m_block;

    /// @brief The `block` of the else part of the `if` statement.
    std::optional<std::unique_ptr<StmtBlock>> m_else;

    std::shared_ptr<Token> m_tok;

    StmtIf(std::unique_ptr<Expr> expr,
           std::unique_ptr<StmtBlock> block,
           std::optional<std::unique_ptr<StmtBlock>> else_,
           std::shared_ptr<Token> tok);

    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement Return class
struct StmtReturn : public Stmt {
    /// @brief The expression that the Statement Return is returning
    std::optional<std::unique_ptr<Expr>> m_expr;
    std::shared_ptr<Token> m_tok;

    StmtReturn(std::optional<std::unique_ptr<Expr>> expr, std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement Break class
struct StmtBreak : public Stmt {
    std::shared_ptr<Token> m_tok;

    StmtBreak(std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtContinue : public Stmt {
    std::shared_ptr<Token> m_tok;
    StmtContinue(std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement While class
struct StmtWhile : public Stmt {
    /// @brief The expression to loop while it is true
    std::unique_ptr<Expr> m_expr;

    /// @brief The block of the while loop to loop
    std::unique_ptr<StmtBlock> m_block;

    std::shared_ptr<Token> m_tok;

    StmtWhile(std::unique_ptr<Expr> expr,
              std::unique_ptr<StmtBlock> block,
              std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtLoop : public Stmt {
    std::shared_ptr<Token> m_tok;
    std::unique_ptr<StmtBlock> m_block;

    StmtLoop(std::shared_ptr<Token> tok, std::unique_ptr<StmtBlock> block);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Statement For class
struct StmtForeach : public Stmt {
    /// @brief The identifier of the enumerator variable
    std::vector<std::shared_ptr<Token>> m_enumerators;

    std::unique_ptr<Expr> m_expr;

    /// @brief The block for the loop to execute
    std::unique_ptr<StmtBlock> m_block;

    uint32_t m_attrs;

    std::shared_ptr<Token> m_tok;

    StmtForeach(std::vector<std::shared_ptr<Token>> enumerators,
                std::unique_ptr<Expr> expr,
                std::unique_ptr<StmtBlock> block,
                uint32_t attrs,
                std::shared_ptr<Token> tok);

    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtFor : public Stmt {
    /// @brief The identifier of the enumerator variable
    std::shared_ptr<Token> m_enumerator;

    /// @brief The starting expression
    std::unique_ptr<Expr> m_start;

    /// @brief The ending expression
    std::unique_ptr<Expr> m_end;

    /// @brief The block for the loop to execute
    std::unique_ptr<StmtBlock> m_block;

    std::shared_ptr<Token> m_tok;

    StmtFor(std::shared_ptr<Token> enumerator,
            std::unique_ptr<Expr> start,
            std::unique_ptr<Expr> end,
            std::unique_ptr<StmtBlock> block,
            std::shared_ptr<Token> tok);

    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtImport : public Stmt {
    // std::shared_ptr<Token> m_fp;
    std::shared_ptr<Expr> m_fp;
    std::optional<std::shared_ptr<Token>> m_depth;
    std::optional<std::shared_ptr<Token>> m_as;
    uint32_t __m_depth;
    std::shared_ptr<Token> m_tok;

    StmtImport(std::shared_ptr<Expr> fp,
               std::optional<std::shared_ptr<Token>> depth,
               std::optional<std::shared_ptr<Token>> as,
               std::shared_ptr<Token> tok);

    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtUse : public Stmt {
    std::unique_ptr<Expr> m_fp;
    std::optional<std::shared_ptr<Token>> m_as;
    std::shared_ptr<Token> m_tok;

    StmtUse(std::unique_ptr<Expr> fp,
            std::optional<std::shared_ptr<Token>> as,
            std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtMod : public Stmt {
    std::shared_ptr<Token> m_id;

    StmtMod(std::shared_ptr<Token> id);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtClass : public Stmt {
    std::shared_ptr<Token> m_id;
    uint32_t m_attrs;
    std::vector<std::pair<std::shared_ptr<Token>,
                          std::optional<std::shared_ptr<__Type>>>>
    m_constructor_args;

    std::vector<std::unique_ptr<StmtLet>> m_members;
    std::vector<std::unique_ptr<StmtDef>> m_methods;
    std::vector<std::string> m_info;

    std::shared_ptr<Token> m_tok;

    StmtClass(std::shared_ptr<Token> id,
              uint32_t attrs,
              std::vector<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>> constructor_args,
              std::vector<std::unique_ptr<StmtLet>> members,
              std::vector<std::unique_ptr<StmtDef>> methods,
              std::vector<std::string> info,
              std::shared_ptr<Token> tok);

    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtMatch : public Stmt {
    struct Branch {
        Branch(std::vector<std::unique_ptr<Expr>> expr,
               std::optional<std::unique_ptr<Expr>> when,
               std::unique_ptr<StmtBlock> block);

        ~Branch() = default;

        // m_expr is a vector to accomodate the `|`
        // pattern as there can be multiple expressions.
        std::vector<std::unique_ptr<Expr>> m_expr;
        std::optional<std::unique_ptr<Expr>> m_when;
        std::unique_ptr<StmtBlock> m_block;
    };

    std::unique_ptr<Expr> m_expr;
    std::vector<std::unique_ptr<Branch>> m_branches;
    std::shared_ptr<Token> m_tok;

    StmtMatch(std::unique_ptr<Expr> expr,
              std::vector<std::unique_ptr<Branch>> branches,
              std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

struct StmtEnum : public Stmt {
    std::shared_ptr<Token> m_id;
    std::vector<std::pair<std::shared_ptr<Token>, std::unique_ptr<Expr>>> m_elems;
    uint32_t m_attrs;
    std::vector<std::string> m_info;
    std::shared_ptr<Token> m_tok;

    StmtEnum(std::shared_ptr<Token> id,
             std::vector<std::pair<std::shared_ptr<Token>,
             std::unique_ptr<Expr>>> elems,
             uint32_t attrs,
             std::vector<std::string> info,
             std::shared_ptr<Token> tok);
    StmtType stmt_type() const override;
    size_t get_lineno() const override;
};

/// @brief The Program class. It is the starting point
/// of the whole program.
struct Program {
    /// @brief A vector of statement to evaluate
    std::vector<std::unique_ptr<Stmt>> m_stmts;
    const std::string m_filepath;

    Program(std::vector<std::unique_ptr<Stmt>> stmts, const std::string filepath);
};

#endif // AST_H
