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

#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>
#include <memory>

#include "parser.hpp"
#include "utils.hpp"
#include "interpreter.hpp"
#include "intrinsics.hpp"
#include "err.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "ctx.hpp"
#include "common.hpp"
#include "earl.hpp"
#include "lexer.hpp"

using namespace Interpreter;

std::shared_ptr<earl::value::Obj>
eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx);

std::shared_ptr<earl::value::Obj>
eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx);

static std::shared_ptr<earl::value::Obj>
unpack_ER(ER &er, std::shared_ptr<Ctx> &ctx);

static std::shared_ptr<earl::value::Obj>
eval_user_defined_function(const std::string &id,
                           std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx) {
    if (!ctx->function_exists(id))
        ERR_WARGS(Err::Type::Undeclared, "function `%s` has not been defined", id.c_str());
    auto func = ctx->function_get(id);
    auto fctx = std::make_shared<FunctionCtx>();
    func->load_parameters(params, fctx);
    std::shared_ptr<Ctx> mask = fctx;
    return Interpreter::eval_stmt_block(func->block(), mask);
}

static std::vector<std::shared_ptr<earl::value::Obj>>
evaluate_function_parameters(ExprFuncCall *funccall, std::shared_ptr<Ctx> ctx) {
    std::vector<std::shared_ptr<earl::value::Obj>> res = {};
    for (size_t i = 0; i < funccall->m_params.size(); ++i) {
        ER er = Interpreter::eval_expr(funccall->m_params[i].get(), ctx);
        res.push_back(unpack_ER(er, ctx));
    }
    return res;
}

static std::shared_ptr<earl::value::Obj>
unpack_ER(ER &er, std::shared_ptr<Ctx> &ctx) {
    if (er.is_function_ident()) {
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(er.extra), er.ctx);
        if (er.is_intrinsic())
            return Intrinsics::call(er.id, params, ctx);
        else if (er.is_member_intrinsic())
            assert(false);
        return eval_user_defined_function(er.id, params, ctx);
    }
    else if (er.is_literal()) {
        return er.value;
    }
    else if (er.is_ident()) {
        // TODO: implement copy/reference here
        if (!ctx->variable_exists(er.id))
            ERR_WARGS(Err::Type::Fatal, "variable `%s` has not been declared", er.id.c_str());
        return ctx->variable_get(er.id)->value();
    }
    else
        assert(false);
}

ER
eval_expr_term(ExprTerm *expr, std::shared_ptr<Ctx> &ctx) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
        auto ident = dynamic_cast<ExprIdent *>(expr);
        return ER(nullptr, ERT::Ident, /*id=*/ident->m_tok->lexeme());
    } break;
    case ExprTermType::Int_Literal: {
        auto value = std::make_shared<earl::value::Int>(std::stoi(dynamic_cast<ExprIntLit *>(expr)->m_tok->lexeme()));
        return ER(value, ERT::Literal);
    } break;
    case ExprTermType::Str_Literal: {
        auto value = std::make_shared<earl::value::Str>(dynamic_cast<ExprStrLit *>(expr)->m_tok->lexeme());
        return ER(value, ERT::Literal);
    } break;
    case ExprTermType::Char_Literal: {
        UNIMPLEMENTED("ExprTermType::Char_Literal");
    } break;
    case ExprTermType::Func_Call: {
        auto funccall = dynamic_cast<ExprFuncCall *>(expr);
        ER left = Interpreter::eval_expr(funccall->m_left.get(), ctx);
        const std::string &id = left.id;
        if (Intrinsics::is_intrinsic(id))
            return ER(nullptr, static_cast<ERT>(ERT::FunctionIdent|ERT::IntrinsicFunction), /*id=*/id, /*extra=*/static_cast<void *>(funccall), /*ctx=*/ctx);
        if (Intrinsics::is_member_intrinsic(id))
            return ER(nullptr, static_cast<ERT>(ERT::FunctionIdent|ERT::IntrinsicMemberFunction), /*id=*/id, /*extra=*/static_cast<void *>(funccall), /*ctx=*/ctx);
        return ER(nullptr, ERT::FunctionIdent, /*id=*/id, /*extra=*/static_cast<void *>(funccall), /*ctx=*/ctx);
    } break;
    case ExprTermType::List_Literal: {
        assert(false);
    } break;
    case ExprTermType::Get: {
        assert(false);
    } break;
    case ExprTermType::Array_Access: {
        assert(false);
    } break;
    case ExprTermType::Bool: {
        UNIMPLEMENTED("ExprTermType::Bool");
    } break;
    case ExprTermType::None: {
        UNIMPLEMENTED("ExprTermType::None");
    } break;
    case ExprTermType::Closure: {
        UNIMPLEMENTED("ExprTermType::Closure");
    } break;
    case ExprTermType::Tuple: {
        UNIMPLEMENTED("ExprTermType::Tuple");
    } break;
    case ExprTermType::Mod_Access: {
        UNIMPLEMENTED("ExprTermType::Mod_Access");
    } break;
    default:
        ERR_WARGS(Err::Type::Fatal, "unknown term: `%d`", (int)expr->get_term_type());
        assert(false && "unreachable");
    }

    assert(false && "unreachable");
    return ER(nullptr, ERT::None);
}

ER
eval_expr_bin(ExprBinary *expr, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_expr_bin");
}

ER
Interpreter::eval_expr(Expr *expr, std::shared_ptr<Ctx> &ctx) {
    switch (expr->get_type()) {
    case ExprType::Term: {
        auto result = eval_expr_term(dynamic_cast<ExprTerm *>(expr), ctx);
        return result;
    } break;
    case ExprType::Binary: {
        return eval_expr_bin(dynamic_cast<ExprBinary *>(expr), ctx);
    } break;
    default:
        assert(false && "unreachable");
    }
}

std::shared_ptr<earl::value::Obj>
eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx) {
    ER rhs = Interpreter::eval_expr(stmt->m_expr.get(), ctx);

    if (ctx->variable_exists(stmt->m_id->lexeme())) {
        Err::err_wtok(stmt->m_id.get());
        ERR_WARGS(Err::Type::Redeclared, "variable `%s` is already declared", stmt->m_id->lexeme().c_str());
    }

    auto value = unpack_ER(rhs, ctx);
    auto var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), value, stmt->m_attrs);
    ctx->variable_add(var);

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_expr(StmtExpr *stmt, std::shared_ptr<Ctx> &ctx) {
    ER er = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    return unpack_ER(er, ctx);
}

std::shared_ptr<earl::value::Obj>
Interpreter::eval_stmt_block(StmtBlock *block, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> result = nullptr;

    ctx->push_scope();
    for (size_t i = 0; i < block->m_stmts.size(); ++i) {
        result = Interpreter::eval_stmt(block->m_stmts.at(i).get(), ctx);
        if (result && result->type() != earl::value::Type::Void) {
            // We hit either a break or return statement.
            break;
        }
    }
    ctx->pop_scope();

    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx) {
    const std::string &id = stmt->m_id->lexeme();
    if (ctx->function_exists(id)) {
        Err::err_wtok(stmt->m_id.get());
        ERR_WARGS(Err::Type::Redeclared, "function `%s` has already been declared", id.c_str());
    }

    std::vector<std::pair<Token *, uint32_t>> args;
    for (auto &entry : stmt->m_args)
        args.push_back(std::make_pair(entry.first.get(), entry.second));

    auto func = std::make_shared<earl::function::Obj>(stmt, args);
    ctx->function_add(func);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_if(StmtIf *stmt, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_stmt_if");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_return(StmtReturn *stmt, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_stmt_return");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_break(StmtBreak *stmt, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_stmt_break");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_mut(StmtMut *stmt, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_stmt_mut");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_while(StmtWhile *stmt, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_stmt_while");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_for(StmtFor *stmt, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_stmt_for");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_class(StmtClass *stmt, std::shared_ptr<Ctx> &ctx) {
    UNIMPLEMENTED("eval_stmt_class");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_mod(StmtMod *stmt, std::shared_ptr<Ctx> &ctx) {
    dynamic_cast<WorldCtx *>(ctx.get())->set_mod(stmt->m_id->lexeme());
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Interpreter::eval_stmt(Stmt *stmt, std::shared_ptr<Ctx> &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Def: {
        return eval_stmt_def(dynamic_cast<StmtDef *>(stmt), ctx);
    } break;
    case StmtType::Let: {
        return eval_stmt_let(dynamic_cast<StmtLet *>(stmt), ctx);
    } break;
    case StmtType::Block: {
        return Interpreter::eval_stmt_block(dynamic_cast<StmtBlock *>(stmt), ctx);
    } break;
    case StmtType::Mut: {
        return eval_stmt_mut(dynamic_cast<StmtMut *>(stmt), ctx);
    } break;
    case StmtType::Stmt_Expr: {
        return eval_stmt_expr(dynamic_cast<StmtExpr *>(stmt), ctx);
    } break;
    case StmtType::If: {
        return eval_stmt_if(dynamic_cast<StmtIf *>(stmt), ctx);
    } break;
    case StmtType::Return: {
        return eval_stmt_return(dynamic_cast<StmtReturn *>(stmt), ctx);
    } break;
    case StmtType::Break: {
        return eval_stmt_break(dynamic_cast<StmtBreak *>(stmt), ctx);
    } break;
    case StmtType::While: {
        return eval_stmt_while(dynamic_cast<StmtWhile *>(stmt), ctx);
    } break;
    case StmtType::For: {
        return eval_stmt_for(dynamic_cast<StmtFor *>(stmt), ctx);
    } break;
    case StmtType::Import: {
        UNIMPLEMENTED("StmtType::Import");
    } break;
    case StmtType::Mod: {
        return eval_stmt_mod(dynamic_cast<StmtMod *>(stmt), ctx);
    } break;
    case StmtType::Class: {
        return eval_stmt_class(dynamic_cast<StmtClass *>(stmt), ctx);
    } break;
    case StmtType::Match: {
        UNIMPLEMENTED("StmtType::Match");
    } break;
    default:
        assert(false && "unreachable");
    }

    assert(false && "unreachable");
}

std::shared_ptr<Ctx>
Interpreter::interpret(std::unique_ptr<Program> program, std::unique_ptr<Lexer> lexer) {
    std::shared_ptr<Ctx> ctx = std::make_shared<WorldCtx>(std::move(lexer), std::move(program));
    auto wctx = dynamic_cast<WorldCtx *>(ctx.get());

    for (size_t i = 0; i < wctx->stmts_len(); ++i) {
        (void)Interpreter::eval_stmt(wctx->stmt_at(i), ctx);
    }

    return ctx;
}
