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

#include "utils.hpp"
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

std::any Interpreter::ExprEvalResult::value(void) {
    return m_expr_value;
}

EarlTy::Type Interpreter::ExprEvalResult::get_earl_type(Ctx &ctx) {
    if (m_expr_term_type == ExprTermType::Ident) {
        Token *tok = std::any_cast<Token *>(m_expr_value);
        auto *var = ctx.get_registered_earlvar(tok->lexeme());
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

static Interpreter::ExprEvalResult
eval_user_defined_function(ExprFuncCall *expr,
                           std::vector<Interpreter::ExprEvalResult>
                           user_params,
                           Ctx &ctx) {

    auto *func = ctx.get_registered_earlfunc(expr->m_id->lexeme());

    ctx.set_current_earlfunc(func);

    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        Interpreter::ExprEvalResult user_param = user_params[i];
        func->m_args[i]->set_value(user_param.value());
        ctx.register_earlvar(func->m_args[i]);
    }

    Interpreter::ExprEvalResult blockresult = eval_stmt_block(func->m_block, ctx);

    ctx.unset_current_earlfunc();

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
        EarlVar *stored = ctx.get_registered_earlvar(ident->m_tok->lexeme());
        return Interpreter::ExprEvalResult {
            stored->m_value,
            ident->get_term_type(),
            stored->m_type,
            Interpreter::LiteralResult::Result { stored },
        };
    } break;
    case ExprTermType::Int_Literal: {
        ExprIntLit *intlit = dynamic_cast<ExprIntLit *>(expr);
        return Interpreter::ExprEvalResult {
            std::stoi(intlit->m_tok->lexeme()),
            intlit->get_term_type(),
            EarlTy::Type::Int,
            Interpreter::LiteralResult::Result { intlit->m_tok.get() },
        };
    } break;
    case ExprTermType::Str_Literal: {
        ExprStrLit *strlit = dynamic_cast<ExprStrLit *>(expr);
        return Interpreter::ExprEvalResult {
            strlit->m_tok->lexeme(),
            strlit->get_term_type(),
            EarlTy::Type::Str,
            Interpreter::LiteralResult::Result { strlit->m_tok.get() },
        };
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

    if (!EarlTy::earlvar_type_compat(lhs.m_earl_type, rhs.m_earl_type)) {
        ERR_WARGS(ErrType::ERR_FATAL, "type (%d) is not compatable with type (%d)",
                  static_cast<int>(lhs.m_earl_type), static_cast<int>(rhs.m_earl_type));
    }

    switch (expr->m_op->type()) {
    case TokenType::Plus: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) + std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Minus: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) - std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Asterisk: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) * std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Percent: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) % std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Forwardslash: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) / std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Double_Equals: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) == std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Bang_Equals: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) != std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Double_Ampersand: {
        return Interpreter::ExprEvalResult {
            std::any_cast<bool>(lhs.value()) && std::any_cast<bool>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    case TokenType::Lessthan: {
        return Interpreter::ExprEvalResult {
            std::any_cast<int>(lhs.value()) < std::any_cast<int>(rhs.value()),
            ExprTermType::Int_Literal,
            lhs.m_earl_type,
            Interpreter::LiteralResult::Result {false},
        };
    } break;
    default:
        ERR_WARGS(ErrType::Fatal, "%s is not a valid binary operator", expr->m_op->lexeme().c_str());
    }

    // Unreachable
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

    if (ctx.is_registered_earlvar(id)) {
        ERR_WARGS(ErrType::Redeclared, "variable `%s` is already defined", id.c_str());
    }

    // The `let` type binding i.e., let x: <TYPE> = ...;
    EarlTy::Type binding_type = EarlTy::of_str(stmt->m_type->lexeme());

    Interpreter::ExprEvalResult expr_eval = Interpreter::eval_expr(stmt->m_expr.get(), ctx);

    // The type of the right side of the equals sign
    EarlTy::Type rval_type = expr_eval.m_earl_type;

    if (!EarlTy::earlvar_type_compat(binding_type, rval_type)) {
        ERR_WARGS(ErrType::ERR_FATAL, "type (%d) is not compatable with type (%d)",
                  static_cast<int>(binding_type), static_cast<int>(rval_type));
    }

    EarlVar *var = new EarlVar(stmt->m_id.get(), binding_type, false, expr_eval.m_expr_value);
    ctx.register_earlvar(var);

    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_expr(StmtExpr *stmt, Ctx &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx);
}

Interpreter::ExprEvalResult eval_stmt_block(StmtBlock *block, Ctx &ctx) {
    Interpreter::ExprEvalResult result{};

    ctx.push_scope();
    for (auto &stmt : block->m_stmts) {
        result = eval_stmt(stmt.get(), ctx);
        if (result.value().has_value())
            break;
    }
    ctx.pop_scope();

    return result;
}

// When we hit a statement `def` (a function declaration),
// we do not actually want to execute this function.
// We just want to add it to the global context so it
// can be called later from either a statement expression
// or a right-hand-side assignment.
Interpreter::ExprEvalResult eval_stmt_def(StmtDef *stmt, Ctx &ctx) {
    std::vector<EarlVar *> args;
    for (auto &arg : stmt->m_args) {
        Token *id = arg.first.get();
        EarlTy::Type type = EarlTy::of_str(arg.second->lexeme());
        args.push_back(new EarlVar(id, type, false, nullptr));
    }

    auto *func = new EarlFunc::Func(stmt->m_id.get(),
                                    EarlTy::of_str(stmt->m_rettype->lexeme()),
                                    std::move(args),
                                    stmt->m_block.get());

    ctx.register_earlfunc(func);

    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_if(StmtIf *stmt, Ctx &ctx) {
    Interpreter::ExprEvalResult expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    Interpreter::ExprEvalResult result{};

    if (std::any_cast<bool>(expr_result.value()) == true) {
        result = eval_stmt_block(stmt->m_block.get(), ctx);
    }
    else if (stmt->m_else.has_value()) {
        result = eval_stmt_block(stmt->m_else.value().get(), ctx);
    }

    return result;
}

Interpreter::ExprEvalResult eval_stmt_return(StmtReturn *stmt, Ctx &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx);
}

Interpreter::ExprEvalResult eval_stmt_mut(StmtMut *stmt, Ctx &ctx) {
    Interpreter::ExprEvalResult left = Interpreter::eval_expr(stmt->m_left.get(), ctx);
    Interpreter::ExprEvalResult right = Interpreter::eval_expr(stmt->m_right.get(), ctx);

    EarlVar *var = std::get<EarlVar *>(left.m_literal_result.m_value);
    assert(ctx.is_registered_earlvar(var->m_id->lexeme()));

    if (!EarlTy::earlvar_type_compat(left.m_earl_type, right.m_earl_type)) {
        ERR_WARGS(ErrorType::Fatal,
                  "type %d is not compatable with type %d",
                  static_cast<int>(left.m_earl_type),
                  static_cast<int>(right.m_earl_type));
    }

    var = ctx.get_registered_earlvar(var->m_id->lexeme());
    var->set_value(right.value());

    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_while(StmtWhile *stmt, Ctx &ctx) {
    Interpreter::ExprEvalResult expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    Interpreter::ExprEvalResult result{};

    while (std::any_cast<bool>(expr_result.value()) == true) {
        result = eval_stmt_block(stmt->m_block.get(), ctx);
        if (result.value().has_value())
            break;
        expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    }

    return result;
}

Interpreter::ExprEvalResult eval_stmt_for(StmtFor *stmt, Ctx &ctx) {

    Interpreter::ExprEvalResult result{};

    Interpreter::ExprEvalResult start_expr = Interpreter::eval_expr(stmt->m_start.get(), ctx);
    Interpreter::ExprEvalResult end_expr = Interpreter::eval_expr(stmt->m_end.get(), ctx);

    EarlVar *enumerator = new EarlVar(stmt->m_enumerator.get(), EarlTy::Type::Int, false, std::any_cast<int>(start_expr.value()));
    assert(!ctx.is_registered_earlvar(enumerator->m_id->lexeme()));
    ctx.register_earlvar(enumerator);

    while (std::any_cast<int>(enumerator->m_value) < std::any_cast<int>(end_expr.value())) {
        result = eval_stmt_block(stmt->m_block.get(), ctx);

        if (result.value().has_value())
            break;

        enumerator->m_value = std::any_cast<int>(enumerator->m_value)+1;
    }

    // Remove the enumerator as it is only a tmp variable.
    ctx.deregister_earlvar(enumerator);
    delete enumerator;

    return result;
}

Interpreter::ExprEvalResult eval_stmt(Stmt *stmt, Ctx &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Let: {
        return eval_stmt_let(dynamic_cast<StmtLet *>(stmt), ctx);
    } break;
    case StmtType::Mut: {
        return eval_stmt_mut(dynamic_cast<StmtMut *>(stmt), ctx);
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
    case StmtType::If: {
        return eval_stmt_if(dynamic_cast<StmtIf *>(stmt), ctx);
    } break;
    case StmtType::Stmt_Return: {
        return eval_stmt_return(dynamic_cast<StmtReturn *>(stmt), ctx);
    } break;
    case StmtType::Stmt_While: {
        return eval_stmt_while(dynamic_cast<StmtWhile *>(stmt), ctx);
    } break;
    case StmtType::Stmt_For: {
        return eval_stmt_for(dynamic_cast<StmtFor *>(stmt), ctx);
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
