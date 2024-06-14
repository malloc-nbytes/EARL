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
#include "common.hpp"
#include "earl.hpp"

earl::value::Obj *eval_stmt(Stmt *stmt, Ctx &ctx);
earl::value::Obj *eval_stmt_block(StmtBlock *block, Ctx &ctx);

earl::value::Obj *eval_user_defined_function(earl::function::Obj *func, std::vector<earl::value::Obj *> params, Ctx &ctx) {
    func->load_parameters(params);

    eval_stmt_block(func->block(), ctx);
}

earl::value::Obj *eval_expr_funccall(ExprFuncCall *expr, Ctx &ctx) {
    std::vector<earl::value::Obj *> params;
    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        params.push_back(Interpreter::eval_expr(expr->m_params.at(i).get(), ctx));
    }

    if (Intrinsics::is_intrinsic(expr->m_id->lexeme())) {
        return Intrinsics::call(expr, params, ctx);
    }

    earl::function::Obj *func = ctx.get_registered_function(expr->m_id->lexeme());
    return eval_user_defined_function(func, params, ctx);
}

earl::value::Obj *eval_expr_term(ExprTerm *expr, Ctx &ctx) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
        ExprIdent *ident = dynamic_cast<ExprIdent *>(expr);
        earl::variable::Obj *stored = ctx.get_registered_variable(ident->m_tok->lexeme());
        return stored->value();
    } break;
    case ExprTermType::Int_Literal: {
        ExprIntLit *intlit = dynamic_cast<ExprIntLit *>(expr);
        return new earl::value::Int(std::stoi(intlit->m_tok->lexeme()));
    } break;
    case ExprTermType::Str_Literal: {
        ExprStrLit *strlit = dynamic_cast<ExprStrLit *>(expr);
        return new earl::value::Str(strlit->m_tok->lexeme());
    } break;
    case ExprTermType::Func_Call: {
        return eval_expr_funccall(dynamic_cast<ExprFuncCall *>(expr), ctx);
    } break;
    default: {
        ERR_WARGS(Err::Type::Fatal, "unknown expression term type %d", static_cast<int>(expr->get_term_type()));
    } break;
    }
}

earl::value::Obj *eval_expr_bin(ExprBinary *expr, Ctx &ctx) {
    UNIMPLEMENTED("eval_expr_bin");
}

earl::value::Obj *Interpreter::eval_expr(Expr *expr, Ctx &ctx) {
    switch (expr->get_type()) {
    case ExprType::Term: {
        return eval_expr_term(dynamic_cast<ExprTerm *>(expr), ctx);
    } break;
    case ExprType::Binary: {
        return eval_expr_bin(dynamic_cast<ExprBinary *>(expr), ctx);
    } break;
    default: {
        ERR_WARGS(Err::Type::Fatal, "unknown expr type %d", static_cast<int>(expr->get_type()));
    } break;
    }
}

earl::value::Obj *eval_stmt_let(StmtLet *stmt, Ctx &ctx) {
    if (ctx.variable_is_registered(stmt->m_id->lexeme())) {
        ERR_WARGS(Err::Type::Redeclared,
                  "variable `%s` is already declared", stmt->m_id->lexeme().c_str());
    }

    earl::value::Obj *binding_type = earl::value::of_str(stmt->m_type.get()->lexeme());

    earl::value::Obj *rhs_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);

    if (!earl::value::type_is_compatable(binding_type, rhs_result)) {
        Err::err_wtok(stmt->m_id.get());
        ERR(Err::Type::Fatal, "binding type does not match the evaluated expression type");
    }

    if (!earl::value::type_is_compatable(binding_type, rhs_result)) {
        Err::err_wtok(stmt->m_id.get());
        ERR(Err::Type::Fatal, "binding type does not match the evaluated expression type");
    }

    earl::variable::Obj *created_variable =
        new earl::variable::Obj(stmt->m_id.get(), std::unique_ptr<earl::value::Obj>(rhs_result));

    ctx.register_variable(created_variable);

    return new earl::value::Void();
}

earl::value::Obj *eval_stmt_expr(StmtExpr *stmt, Ctx &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx);
}

earl::value::Obj *eval_stmt_block(StmtBlock *block, Ctx &ctx) {
    UNIMPLEMENTED("eval_stmt_block");
}

// When we hit a statement `def` (a function declaration),
// we do not actually want to execute this function.
// We just want to add it to the global context so it
// can be called later from either a statement expression
// or a right-hand-side assignment.
earl::value::Obj *eval_stmt_def(StmtDef *stmt, Ctx &ctx) {
    if (ctx.function_is_registered(stmt->m_id->lexeme())) {
        ERR_WARGS(Err::Type::Redeclared,
                  "function `%s` is already declared", stmt->m_id->lexeme().c_str());
    }
    earl::function::Obj *created_function = new earl::function::Obj(stmt);
    ctx.register_function(created_function);
    return new earl::value::Void();
}

earl::value::Obj *eval_stmt_if(StmtIf *stmt, Ctx &ctx) {
    UNIMPLEMENTED("eval_stmt_if");
}

earl::value::Obj *eval_stmt_return(StmtReturn *stmt, Ctx &ctx) {
    UNIMPLEMENTED("eval_stmt_return");
}

earl::value::Obj *eval_stmt_mut(StmtMut *stmt, Ctx &ctx) {
    UNIMPLEMENTED("eval_stmt_mut");
}

earl::value::Obj *eval_stmt_while(StmtWhile *stmt, Ctx &ctx) {
    UNIMPLEMENTED("eval_stmt_while");
}

earl::value::Obj *eval_stmt_for(StmtFor *stmt, Ctx &ctx) {
    UNIMPLEMENTED("eval_stmt_for");
}

earl::value::Obj *eval_stmt(Stmt *stmt, Ctx &ctx) {
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
}

earl::value::Obj *Interpreter::interpret(Program &program) {
    Ctx ctx;
    earl::value::Obj *meta;

    for (size_t i = 0; i < program.m_stmts.size(); ++i) {
        meta = eval_stmt(program.m_stmts.at(i).get(), ctx);
    }

    return new earl::value::Void();
}
