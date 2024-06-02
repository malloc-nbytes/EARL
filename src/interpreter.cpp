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

Interpreter::ExprEvalResult eval_stmt(std::unique_ptr<Stmt> stmt, Ctx &ctx);
Interpreter::ExprEvalResult eval_stmt_block(StmtBlock *block, Ctx &ctx);

// Used when an ExprEvalResult has a expression term type
// of `ident` to copy over the information from the ident
// that it is assigned into it's actual value.
// NOTE: expr.get_earl_type() should be called before calling
// this function to properly get the correct EARL type.
static void try_copy_ident_value(Interpreter::ExprEvalResult &expr_eval, Ctx &ctx) {
    (void)expr_eval;
    (void)ctx;
}

EarlTy::Type Interpreter::ExprEvalResult::get_earl_type(Ctx &ctx) {
    (void)ctx;
    return EarlTy::Type::Int;
}

static Interpreter::ExprEvalResult eval_user_defined_function(ExprFuncCall *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_expr_funccall(ExprFuncCall *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_expr_term(ExprTerm *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_expr_bin(ExprBinary *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    return Interpreter::ExprEvalResult {};
}

Interpreter::ExprEvalResult Interpreter::eval_expr(Expr *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_let(StmtLet *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_expr(StmtExpr *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt_block(StmtBlock *block, Ctx &ctx) {
    (void)block;
    (void)ctx;
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

    // ctx
    //     .add_earlfunc_to_scope(std::make_unique<EarlFunc>(std::move(stmt->m_id),
    //                                                       EarlTy::of_str(stmt->m_rettype->lexeme()),
    //                                                       std::move(args),
    //                                                       std::move(stmt->m_block)));
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult eval_stmt(std::unique_ptr<Stmt> stmt, Ctx &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Let: {
        return eval_stmt_let(dynamic_cast<StmtLet *>(stmt.get()), ctx);
    } break;
    case StmtType::Mut: {
        assert(false && "unimplemented");
    } break;
    case StmtType::Def: {
        return eval_stmt_def(dynamic_cast<StmtDef *>(stmt.get()), ctx);
    } break;
    case StmtType::Block: {
        assert(false && "unimplemented");
    } break;
    case StmtType::Stmt_Expr: {
        return eval_stmt_expr(dynamic_cast<StmtExpr *>(stmt.get()), ctx);
    } break;
    default:
        assert(false && "eval_stmt: invalid statement");
    }
    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult Interpreter::interpret(Program &program) {
    Ctx ctx;

    for (size_t i = 0; i < program.m_stmts.size(); ++i) {
        eval_stmt(std::move(program.m_stmts.at(i)), ctx);
    }

    return Interpreter::ExprEvalResult{};
}
