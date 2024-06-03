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

#include <any>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>

#include "interpreter.hpp"
#include "intrinsics.hpp"
#include "err.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "ctx.hpp"
#include "earlvar.hpp"
#include "common.hpp"

Interpreter::ExprEvalResult eval_stmt(Stmt *stmt, Ctx &ctx);
Interpreter::ExprEvalResult eval_stmt_block(StmtBlock *block, Ctx &ctx);

// Used when an ExprEvalResult has a expression term type
// of `ident` to copy over the information from the ident
// that it is assigned into it's actual value.
// NOTE: expr.get_earl_type() should be called before calling
// this function to properly get the correct EARL type.
static void try_copy_ident_value(Interpreter::ExprEvalResult &expr_eval, bool get_global, Ctx &ctx) {
    if (expr_eval.m_expr_term_type == ExprTermType::Ident) {
        const std::string &id = std::any_cast<Token *>(expr_eval.m_expr_value)->lexeme();
        EarlVar *var = nullptr;
        if (get_global)
            var = ctx.get_global_earlvar_from_scope(id);
        else
            var = ctx.get_earlvar_from_scope(id);
        expr_eval.m_expr_value = var->m_value;
    }
}

EarlTy::Type Interpreter::ExprEvalResult::get_earl_type(Ctx &ctx) {
    if (m_expr_term_type == ExprTermType::Ident) {
        Token *tok = std::any_cast<Token *>(m_expr_value);
        if (!ctx.earlvar_in_scope(tok->lexeme())) {
            ERR_WARGS(ErrType::Fatal, "variable `%s` is not in scope", tok->lexeme().c_str());
        }
        EarlVar *var = ctx.get_earlvar_from_scope(tok->lexeme());
        return var->m_type;
    }

    switch (m_expr_term_type) {
    case ExprTermType::Int_Literal: return EarlTy::Type::Int;
    case ExprTermType::Str_Literal: return EarlTy::Type::Str;
    default:
        ERR_WARGS(ErrType::Fatal, "ExprTermType `%d` is not a valid EARL type",
                  static_cast<int>(m_expr_term_type));
    }
}

static Interpreter::ExprEvalResult eval_user_defined_function(ExprFuncCall *expr, std::vector<Interpreter::ExprEvalResult> params, Ctx &ctx) {
    EarlFunc::Func *func = ctx.get_earlfunc_from_scope(expr->m_id->lexeme());

    // ctx.set_current_earlfunc(func);
    // ctx.push_scope();

    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        Interpreter::ExprEvalResult param = params[i];

        if (param.m_expr_term_type == ExprTermType::Ident) {
            Token *tok = std::any_cast<Token *>(param.m_expr_value);
            EarlVar *var = ctx.get_earlvar_from_scope(tok->lexeme());
            func->m_args[i]->m_value = var->m_value;
        }

        else {
            func->m_args[i]->m_value = param.m_expr_value;
        }

        ctx.add_earlvar_to_scope(std::move(func->m_args[i]));
    }

    Interpreter::ExprEvalResult blockresult = eval_stmt_block(func->m_block, ctx);
    // ctx.pop_scope();
    // ctx.unset_current_earlfunc();

    return blockresult;
}

Interpreter::ExprEvalResult eval_expr_funccall(ExprFuncCall *expr, Ctx &ctx) {
    std::vector<Interpreter::ExprEvalResult> arg_evals;
    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        std::unique_ptr<Expr> &e = expr->m_params[i];
        Interpreter::ExprEvalResult param = Interpreter::eval_expr(e.get(), ctx);
        arg_evals.push_back(param);
    }

    if (Intrinsics::is_intrinsic_function(expr->m_id->lexeme())) {
        return Intrinsics::run_intrinsic_function(expr, arg_evals, ctx);
    }
    return eval_user_defined_function(expr, arg_evals, ctx);
}

Interpreter::ExprEvalResult eval_expr_term(ExprTerm *expr, Ctx &ctx) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
        ExprIdent *ident = dynamic_cast<ExprIdent *>(expr);
        if (!ctx.earlvar_in_scope(ident->m_tok->lexeme().c_str())) {
            ERR_WARGS(ErrType::Undeclared, "variable `%s` is not in scope", ident->m_tok->lexeme().c_str());
        }
        return Interpreter::ExprEvalResult {ident->m_tok.get(), ident->get_term_type()};
    } break;
    case ExprTermType::Int_Literal: {
        ExprIntLit *intlit = dynamic_cast<ExprIntLit *>(expr);
        return Interpreter::ExprEvalResult {std::stoi(intlit->m_tok->lexeme()), intlit->get_term_type()};
    } break;
    case ExprTermType::Str_Literal: {
        ExprStrLit *strlit = dynamic_cast<ExprStrLit *>(expr);
        return Interpreter::ExprEvalResult {strlit->m_tok->lexeme(), strlit->get_term_type()};
    } break;
    case ExprTermType::Func_Call: {
        return eval_expr_funccall(dynamic_cast<ExprFuncCall *>(expr), ctx);
    } break;
    default:
        ERR_WARGS(ErrType::Fatal, "%d is not a valid expression term type is not valid",
                  static_cast<int>(expr->get_term_type()));
    }
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_expr_bin(ExprBinary *expr, Ctx &ctx) {
    Interpreter::ExprEvalResult lhs = Interpreter::eval_expr(expr->m_lhs.get(), ctx);
    Interpreter::ExprEvalResult rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx);

    EarlTy::Type lhs_type = lhs.get_earl_type(ctx);
    EarlTy::Type rhs_type = rhs.get_earl_type(ctx);

    try_copy_ident_value(lhs, false, ctx);
    try_copy_ident_value(rhs, false, ctx);

    if (!EarlTy::earlvar_type_compat(lhs_type, rhs_type)) {
        ERR_WARGS(ErrType::ERR_FATAL, "type (%d) is not compatable with type (%d)",
                  static_cast<int>(lhs_type), static_cast<int>(rhs_type));
    }

    switch (expr->m_op->type()) {
    case TokenType::Plus: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.m_expr_value) + std::any_cast<int>(rhs.m_expr_value),
            ExprTermType::Int_Literal
        };
    } break;
    case TokenType::Minus: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.m_expr_value) - std::any_cast<int>(rhs.m_expr_value),
            ExprTermType::Int_Literal
        };
    } break;
    case TokenType::Asterisk: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.m_expr_value) * std::any_cast<int>(rhs.m_expr_value),
            ExprTermType::Int_Literal
        };
    } break;
    case TokenType::Forwardslash: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.m_expr_value) / std::any_cast<int>(rhs.m_expr_value),
            ExprTermType::Int_Literal
        };
    } break;
    default:
        ERR_WARGS(ErrType::Fatal, "%s is not a valid binary operator", expr->m_op->lexeme().c_str());
    }

    return Interpreter::ExprEvalResult {};
}

Interpreter::ExprEvalResult Interpreter::eval_expr(Expr *expr, Ctx &ctx) {
    switch (expr->get_type()) {
    case ExprType::Term: {
        return eval_expr_term(dynamic_cast<ExprTerm *>(expr), ctx);
    } break;
    case ExprType::Binary: {
        return eval_expr_bin(dynamic_cast<ExprBinary *>(expr), ctx);
    } break;
    default:
        ERR_WARGS(ErrType::Fatal, "expression type %d is not a valid expression",
                  static_cast<int>(expr->get_type()));
    }
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_let(StmtLet *stmt, Ctx &ctx) {
    const std::string &id = stmt->m_id->lexeme();
    if (ctx.earlvar_in_scope(id)) {
        ERR_WARGS(ErrType::Redeclared, "variable `%s` is already defined", id.c_str());
    }

    // The `let` type binding i.e., let x: <TYPE> = ...;
    EarlTy::Type binding_type = EarlTy::of_str(stmt->m_type->lexeme());

    Interpreter::ExprEvalResult expr_eval = Interpreter::eval_expr(stmt->m_expr.get(), ctx);

    // The type of the right side of the equals sign
    EarlTy::Type rval_type = expr_eval.get_earl_type(ctx);

    try_copy_ident_value(expr_eval, false, ctx);

    if (!EarlTy::earlvar_type_compat(binding_type, rval_type)) {
        ERR_WARGS(ErrType::ERR_FATAL, "type (%d) is not compatable with type (%d)",
                  static_cast<int>(binding_type), static_cast<int>(rval_type));
    }

    ctx.add_earlvar_to_scope(std::make_unique<EarlVar>(stmt->m_id.get(), binding_type, false, expr_eval.m_expr_value));

    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_expr(StmtExpr *stmt, Ctx &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx);
}

Interpreter::ExprEvalResult eval_stmt_block(StmtBlock *block, Ctx &ctx) {
    for (auto &stmt : block->m_stmts) {
        eval_stmt(stmt.get(), ctx);
    }

    return Interpreter::ExprEvalResult{};
}

// When we hit a statement `def` (a function declaration),
// we do not actually want to execute this function.
// We just want to add it to the global context so it
// can be called later from either a statement expression
// or a right-hand-side assignment.
Interpreter::ExprEvalResult eval_stmt_def(StmtDef *stmt, Ctx &ctx) {
    std::vector<std::unique_ptr<EarlVar>> args;

    for (auto &arg : stmt->m_args) {
        Token *id = arg.first.get();
        EarlTy::Type type = EarlTy::of_str(arg.second->lexeme());
        args.push_back(std::make_unique<EarlVar>(id, type, false, nullptr));
    }

    ctx.add_earlfunc_to_scope(std::make_unique<EarlFunc::Func>(stmt->m_id.get(),
                                                               EarlTy::of_str(stmt->m_rettype->lexeme()),
                                                               std::move(args),
                                                               stmt->m_block.get()));
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt(Stmt *stmt, Ctx &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Let: {
        return eval_stmt_let(dynamic_cast<StmtLet *>(stmt), ctx);
    } break;
    case StmtType::Mut: {
        assert(false && "unimplemented");
    } break;
    case StmtType::Def: {
        return eval_stmt_def(dynamic_cast<StmtDef *>(stmt), ctx);
    } break;
    case StmtType::Block: {
        assert(false && "unimplemented");
    } break;
    case StmtType::Stmt_Expr: {
        return eval_stmt_expr(dynamic_cast<StmtExpr *>(stmt), ctx);
    } break;
    default:
        assert(false && "eval_stmt: invalid statement");
    }
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult Interpreter::interpret(Program &program) {
    Ctx ctx;

    for (size_t i = 0; i < program.m_stmts.size(); ++i) {
        eval_stmt(program.m_stmts.at(i).get(), ctx);
    }

    return Interpreter::ExprEvalResult{};
}
