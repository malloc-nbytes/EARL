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
#include <optional>
#include <functional>

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

struct PackedERPreliminary {
    std::shared_ptr<earl::value::Obj> lhs_getter_accessor;
    PackedERPreliminary(std::shared_ptr<earl::value::Obj> lhs_get = nullptr)
        : lhs_getter_accessor(lhs_get) {}
};

static std::shared_ptr<earl::value::Obj>
eval_user_defined_function(const std::string &id,
                           std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           bool from_outside = false);

std::shared_ptr<earl::value::Obj>
eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx);

std::shared_ptr<earl::value::Obj>
eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx);

static std::shared_ptr<earl::value::Obj>
unpack_ER(ER &er, std::shared_ptr<Ctx> &ctx, bool ref, PackedERPreliminary *perp = nullptr);

static std::shared_ptr<earl::value::Obj>
eval_stmt_let_wcustom_buffer(StmtLet *stmt,
                             std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> &buffer,
                             std::shared_ptr<Ctx> &ctx, bool ref) {
    ER rhs = Interpreter::eval_expr(stmt->m_expr.get(), ctx, ref);
    std::shared_ptr<earl::value::Obj> value = nullptr;

    if (rhs.is_ident() && buffer.find(rhs.id) != buffer.end())
        value = buffer.find(rhs.id)->second->value();
    else
        value = unpack_ER(rhs, ctx, ref);

    std::shared_ptr<earl::variable::Obj> var = nullptr;

    if (rhs.is_literal() || (rhs.is_ident() && ((stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0)))
        // If the result is a literal, no need to copy.
        // If the result is not a literal, but we have a `ref` attr, no need to copy.
        var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), value, stmt->m_attrs);
    else
        // Copy the value
        var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), value->copy(), stmt->m_attrs);

    ctx->variable_add(var);

    return std::make_shared<earl::value::Void>();
}

static std::shared_ptr<earl::value::Obj>
eval_class_instantiation(const std::string &id, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx, bool ref) {
    StmtClass *class_stmt = nullptr;

    if (ctx->type() == CtxType::Class) {
        auto owner_ctx = dynamic_cast<WorldCtx *>(dynamic_cast<ClassCtx *>(ctx.get())->get_owner().get());
        assert(owner_ctx);
        class_stmt = owner_ctx->class_get(id);
    }
    else if (ctx->type() == CtxType::Function) {
        auto class_ctx = dynamic_cast<FunctionCtx *>(ctx.get())->get_outer_class_owner_ctx();

        assert(class_ctx);
        assert(class_ctx->type() == CtxType::Class);

        auto world_ctx = dynamic_cast<WorldCtx *>(dynamic_cast<ClassCtx *>(class_ctx.get())->get_owner().get());

        assert(world_ctx);
        assert(world_ctx->type() == CtxType::World);

        class_stmt = world_ctx->class_get(id);
    }
    else
        class_stmt = dynamic_cast<WorldCtx *>(ctx.get())->class_get(id);

    auto class_ctx = std::make_shared<ClassCtx>(ctx);

    if (params.size() != class_stmt->m_constructor_args.size())
        ERR_WARGS(Err::Type::Fatal, "Class `%s` expects %zu arguments but %zu were supplied",
                  id.c_str(), class_stmt->m_constructor_args.size(), params.size());

    auto klass = std::make_shared<earl::value::Class>(class_stmt, class_ctx);

    std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> buffer;

    // Add the constructor arguments to a temporary pushed scope
    for (size_t i = 0; i < class_stmt->m_constructor_args.size(); ++i) {
        auto var = std::make_shared<earl::variable::Obj>(class_stmt->m_constructor_args[i].get(), params[i]);
        buffer.insert({var->id(), var});
    }

    // Eval member variables
    for (auto &member : class_stmt->m_members)
        (void)eval_stmt_let_wcustom_buffer(member.get(), buffer, klass->ctx(), ref);

    const std::string constructor_id = "constructor";
    bool has_constructor = false;

    // Eval methods
    for (size_t i = 0; i < class_stmt->m_methods.size(); ++i) {
        (void)eval_stmt_def(class_stmt->m_methods[i].get(), klass->ctx());
        if (class_stmt->m_methods[i]->m_id->lexeme() == constructor_id)
            has_constructor = true;
    }

    if (has_constructor) {
        std::vector<std::shared_ptr<earl::value::Obj>> unused = {};
        (void)eval_user_defined_function(constructor_id, unused, klass->ctx());
    }

    return klass;
}

static std::shared_ptr<earl::value::Obj>
eval_user_defined_function(const std::string &id,
                           std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx, bool from_outside) {

    if (ctx->function_exists(id)) {
        auto func = ctx->function_get(id);

        if (from_outside && !func->is_pub())
            ERR_WARGS(Err::Type::Fatal, "function `%s` does not contain the @pub attribute", id.c_str());

        auto fctx = std::make_shared<FunctionCtx>(ctx);
        func->load_parameters(params, fctx);
        std::shared_ptr<Ctx> mask = fctx;
        return Interpreter::eval_stmt_block(func->block(), mask);
    }
    else if (ctx->closure_exists(id)) {
        auto cl = ctx->variable_get(id);
        auto clctx = std::make_shared<ClosureCtx>(ctx);
        auto clvalue = dynamic_cast<earl::value::Closure *>(cl->value().get());
        clvalue->load_parameters(params, clctx);
        std::shared_ptr<Ctx> mask = clctx;
        return Interpreter::eval_stmt_block(clvalue->block(), mask);
    }

    ERR_WARGS(Err::Type::Undeclared, "function `%s` has not been defined", id.c_str());
    return nullptr; // unreachable
}

static std::vector<std::shared_ptr<earl::value::Obj>>
evaluate_function_parameters(ExprFuncCall *funccall, std::shared_ptr<Ctx> ctx, bool ref) {
    std::vector<std::shared_ptr<earl::value::Obj>> res = {};
    for (size_t i = 0; i < funccall->m_params.size(); ++i) {
        ER er = Interpreter::eval_expr(funccall->m_params[i].get(), ctx, ref);
        res.push_back(unpack_ER(er, ctx, ref));
    }
    return res;
}

static std::shared_ptr<earl::value::Obj>
unpack_ER(ER &er, std::shared_ptr<Ctx> &ctx, bool ref, PackedERPreliminary *perp) {
    if (er.is_class_instant()) {
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(er.extra), ctx, ref);
        auto class_instantiation = eval_class_instantiation(er.id, params, ctx, ref);
        return class_instantiation;
    }
    if (er.is_function_ident()) {
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(er.extra), er.ctx, ref);
        if (er.is_intrinsic())
            return Intrinsics::call(er.id, params, ctx);
        if (er.is_member_intrinsic()) {
            assert(perp && perp->lhs_getter_accessor);
            return Intrinsics::call_member(er.id,
                                           perp->lhs_getter_accessor->type(),
                                           perp->lhs_getter_accessor,
                                           params,
                                           ctx);
        }

        if (ctx->type() == CtxType::Class)
            return eval_user_defined_function(er.id, params, ctx);
        return eval_user_defined_function(er.id, params, er.ctx);
    }
    else if (er.is_literal())
        return er.value;
    else if (er.is_ident()) {
        if (!ctx->variable_exists(er.id))
            ERR_WARGS(Err::Type::Fatal, "variable `%s` has not been declared", er.id.c_str());
        auto var = ctx->variable_get(er.id);
        if (!ref)
            return var->value()->copy();
        return var->value();
    }
    else
        assert(false);
}

ER
eval_expr_term_ident(ExprIdent *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    (void)ref;
    const std::string &id = expr->m_tok->lexeme();
    return ER(nullptr, ERT::Ident, /*id=*/id, /*extra=*/nullptr, /*ctx=*/ctx);
}

// RETURNS ACTUAL EVALUATED VALUE IN ER
ER
eval_expr_term_intlit(ExprIntLit *expr) {
    auto value = std::make_shared<earl::value::Int>(std::stoi(expr->m_tok->lexeme()));
    return ER(value, ERT::Literal);
}

// RETURNS ACTUAL EVALUATED VALUE IN ER
ER
eval_expr_term_strlit(ExprStrLit *expr) {
    auto value = std::make_shared<earl::value::Str>(expr->m_tok->lexeme());
    return ER(value, ERT::Literal);
}

ER
eval_expr_term_funccall(ExprFuncCall *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    std::function<std::shared_ptr<Ctx>(const std::string &, std::shared_ptr<Ctx> &)> check_if_is_class
        = [&](const std::string &_id, std::shared_ptr<Ctx> &_ctx) -> std::shared_ptr<Ctx> {
        if (_ctx->type() == CtxType::World && dynamic_cast<WorldCtx *>(_ctx.get())->class_is_defined(_id))
            return _ctx;
        if (_ctx->type() == CtxType::Class)
            return check_if_is_class(_id, dynamic_cast<ClassCtx *>(_ctx.get())->get_owner());
        if (_ctx->type() == CtxType::Function)
            return check_if_is_class(_id, dynamic_cast<FunctionCtx *>(_ctx.get())->get_owner());
        if (_ctx->type() == CtxType::Closure)
            return check_if_is_class(_id, dynamic_cast<ClosureCtx *>(_ctx.get())->get_owner());
        return nullptr;
    };

    ER left = Interpreter::eval_expr(expr->m_left.get(), ctx, ref);
    const std::string &id = left.id;

    if (Intrinsics::is_intrinsic(id))
        return ER(nullptr, static_cast<ERT>(ERT::FunctionIdent|ERT::IntrinsicFunction), /*id=*/id, /*extra=*/static_cast<void *>(expr), /*ctx=*/ctx);

    if (Intrinsics::is_member_intrinsic(id))
        return ER(nullptr, static_cast<ERT>(ERT::FunctionIdent|ERT::IntrinsicMemberFunction), /*id=*/id, /*extra=*/static_cast<void *>(expr), /*ctx=*/ctx);

    std::shared_ptr<Ctx> ctx_wclass = check_if_is_class(id, ctx);
    if (ctx_wclass) {
        return ER(nullptr, static_cast<ERT>(ERT::ClassInstant|ERT::Literal), /*id=*/id, /*extra=*/static_cast<void *>(expr), /*ctx=*/ctx_wclass);

        // auto params = evaluate_function_parameters(expr, ctx, ref);
        // auto value = eval_class_instantiation(id, params, ctx_wclass, ref);
        // return ER(value, static_cast<ERT>(ERT::ClassInstant|ERT::Literal), /*id=*/id, /*extra=*/static_cast<void *>(expr), /*ctx=*/ctx_wclass);
    }

    return ER(nullptr, ERT::FunctionIdent, /*id=*/id, /*extra=*/static_cast<void *>(expr), /*ctx=*/ctx);
}

// RETURNS ACTUAL EVALUATED VALUE IN ER
ER
eval_expr_term_mod_access(ExprModAccess *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    ExprModAccess *mod_access = expr;
    ExprIdent     *left_ident = mod_access->m_expr_ident.get();
    const auto    &left_id    = left_ident->m_tok->lexeme();
    ER right_er(std::shared_ptr<earl::value::Obj>{}, ERT::None);

    std::shared_ptr<Ctx> *ctx_ptr = nullptr;

    if (ctx->type() == CtxType::World) {
        ctx_ptr = dynamic_cast<WorldCtx *>(ctx.get())->get_import(left_id);
    }
    else if (ctx->type() == CtxType::Function) {
        auto world = dynamic_cast<FunctionCtx *>(ctx.get())->get_outer_world_owner();
        ctx_ptr = dynamic_cast<WorldCtx *>(world.get())->get_import(left_id);
    }

    std::shared_ptr<Ctx> &other_ctx = *ctx_ptr;

    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            right_er = Interpreter::eval_expr(arg.get(), *ctx_ptr, true);
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            right_er = Interpreter::eval_expr(arg.get(), *ctx_ptr, true);
        else
            ERR(Err::Type::Internal,
                "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong");
    }, expr->m_right);

    if (right_er.is_class_instant()) {
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(right_er.extra), ctx, ref);
        auto class_instantiation = eval_class_instantiation(right_er.id, params, right_er.ctx, ref);
        return ER(class_instantiation, ERT::Literal, /*id=*/"", /*extra=*/nullptr, /*ctx=*/ctx);
    }
    if (right_er.is_function_ident()) {
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(right_er.extra), ctx, ref);
        auto func = eval_user_defined_function(right_er.id, params, other_ctx, /*from_outside=*/true);
        return ER(func, ERT::Literal);
    }
    else {
        assert(false && "unimplemented");
    }
}

ER
eval_expr_term_get(ExprGet *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    ER left_er = Interpreter::eval_expr(expr->m_left.get(), ctx, ref);
    ER right_er(std::shared_ptr<earl::value::Obj>{}, ERT::None);

    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            right_er = Interpreter::eval_expr(arg.get(), ctx, true);
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            right_er = Interpreter::eval_expr(arg.get(), ctx, true);
        else
            ERR(Err::Type::Internal,
                "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong");
    }, expr->m_right);

    if (left_er.id == "this") {
        if (ctx->type() != CtxType::Function)
            ERR(Err::Type::Fatal, "Must be in a function in a class context to use the `this` keyword");

        auto fctx = dynamic_cast<FunctionCtx *>(ctx.get());

        if (!fctx->in_class())
            ERR(Err::Type::Fatal, "Must be in a class context when using the `this` keyword");

        auto value = unpack_ER(right_er, fctx->get_outer_class_owner_ctx(), /*ref=*/true);
        return ER(value, ERT::Literal);
    }
    else {
        auto left_value = unpack_ER(left_er, ctx, true);
        PackedERPreliminary perp(left_value);

        // The right side (right_er) contains the actual call/identifier to be evaluated,
        // and we need the left (left_value)'s context with the preliminary value of (perp).
        auto value = unpack_ER(right_er, dynamic_cast<earl::value::Class *>(left_value.get())->ctx(), ref, &perp);

        return ER(value, ERT::Literal);
    }

    assert(false && "unreachable");
}

static ER
eval_expr_term_charlit(ExprCharLit *expr) {
    auto value = std::make_shared<earl::value::Char>(expr->m_tok->lexeme());
    return ER(value, ERT::Literal);
}

static ER
eval_expr_term_listlit(ExprListLit *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    std::vector<std::shared_ptr<earl::value::Obj>> list = {};
    for (size_t i = 0; i < expr->m_elems.size(); ++i) {
        ER er = Interpreter::eval_expr(expr->m_elems.at(i).get(), ctx, ref);
        list.push_back(unpack_ER(er, ctx, ref));
    }
    auto value = std::make_shared<earl::value::List>(list);
    return ER(value, ERT::Literal);
}

static ER
eval_expr_term_array_access(ExprArrayAccess *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    ER left_er = Interpreter::eval_expr(expr->m_left.get(), ctx, ref);
    ER idx_er = Interpreter::eval_expr(expr->m_expr.get(), ctx, ref);

    auto left_value = unpack_ER(left_er, ctx, ref);
    auto idx_value = unpack_ER(idx_er, ctx, ref);

    if (left_value->type() == earl::value::Type::List) {
        auto list = dynamic_cast<earl::value::List *>(left_value.get());
        return ER(list->nth(idx_value), ERT::Literal);
    }
    else if (left_value->type() == earl::value::Type::Str) {
        auto str = dynamic_cast<earl::value::Str *>(left_value.get());
        return ER(str->nth(idx_value), ERT::Literal);
    }
    else
        ERR(Err::Type::Fatal, "cannot use `[]` on non-list or non-str type");
}

static ER
eval_expr_term_boollit(ExprBool *expr) {
    auto value = std::make_shared<earl::value::Bool>(expr->m_value);
    return ER(value, ERT::Literal);
}

static ER
eval_expr_term_none(ExprNone *expr) {
    (void)expr;
    auto value = std::make_shared<earl::value::Option>();
    return ER(value, ERT::Literal);
}

static ER
eval_expr_term_closure(ExprClosure *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    std::vector<std::pair<Token *, uint32_t>> args;
    for (auto &entry : expr->m_args)
        args.push_back(std::make_pair(entry.first.get(), entry.second));
    auto cl = std::make_shared<earl::value::Closure>(expr, std::move(args), ctx);
    return ER(cl, ERT::Literal);
}

ER
eval_expr_term(ExprTerm *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident:        return eval_expr_term_ident(dynamic_cast<ExprIdent *>(expr), ctx, ref);
    case ExprTermType::Int_Literal:  return eval_expr_term_intlit(dynamic_cast<ExprIntLit *>(expr));
    case ExprTermType::Str_Literal:  return eval_expr_term_strlit(dynamic_cast<ExprStrLit *>(expr));
    case ExprTermType::Char_Literal: return eval_expr_term_charlit(dynamic_cast<ExprCharLit *>(expr));
    case ExprTermType::Func_Call:    return eval_expr_term_funccall(dynamic_cast<ExprFuncCall *>(expr), ctx, ref);
    case ExprTermType::List_Literal: return eval_expr_term_listlit(dynamic_cast<ExprListLit *>(expr), ctx, ref);
    case ExprTermType::Get:          return eval_expr_term_get(dynamic_cast<ExprGet *>(expr), ctx, ref);
    case ExprTermType::Mod_Access:   return eval_expr_term_mod_access(dynamic_cast<ExprModAccess *>(expr), ctx, ref);
    case ExprTermType::Array_Access: return eval_expr_term_array_access(dynamic_cast<ExprArrayAccess *>(expr), ctx, ref);
    case ExprTermType::Bool:         return eval_expr_term_boollit(dynamic_cast<ExprBool *>(expr));
    case ExprTermType::None:         return eval_expr_term_none(dynamic_cast<ExprNone *>(expr));
    case ExprTermType::Closure:      return eval_expr_term_closure(dynamic_cast<ExprClosure *>(expr), ctx, ref);
    case ExprTermType::Tuple:        UNIMPLEMENTED("ExprTermType::Tuple");
    default:                         ERR_WARGS(Err::Type::Fatal, "unknown term: `%d`", (int)expr->get_term_type());
    }
    assert(false && "unreachable");
    return ER(nullptr, ERT::None);
}

ER
eval_expr_bin(ExprBinary *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    ER lhs = Interpreter::eval_expr(expr->m_lhs.get(), ctx, ref);
    auto lhs_value = unpack_ER(lhs, ctx, true); // POSSIBLE BREAK, WAS FALSE

    // Short-circuit evaluation for logical AND (&&)
    if (expr->m_op->type() == TokenType::Double_Ampersand) {
        // If lhs is false, return lhs (no need to evaluate rhs)
        if (!lhs_value->boolean())
            return lhs;
        ER rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx, ref);
        return ER(unpack_ER(rhs, ctx, ref), ERT::Literal);
    }

    ER rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx, ref);
    auto rhs_value = unpack_ER(rhs, ctx, ref);
    auto result = lhs_value->binop(expr->m_op.get(), rhs_value);
    return ER(result, ERT::Literal);
}

ER
Interpreter::eval_expr(Expr *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    switch (expr->get_type()) {
    case ExprType::Term: {
        auto result = eval_expr_term(dynamic_cast<ExprTerm *>(expr), ctx, ref);
        return result;
    } break;
    case ExprType::Binary: {
        return eval_expr_bin(dynamic_cast<ExprBinary *>(expr), ctx, ref);
    } break;
    default:
        assert(false && "unreachable");
    }
}

std::shared_ptr<earl::value::Obj>
eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx) {
    if (ctx->variable_exists(stmt->m_id->lexeme())) {
        Err::err_wtok(stmt->m_id.get());
        ERR_WARGS(Err::Type::Redeclared, "variable `%s` is already declared", stmt->m_id->lexeme().c_str());
    }

    bool ref = (stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;
    ER rhs = Interpreter::eval_expr(stmt->m_expr.get(), ctx, ref);

    std::shared_ptr<earl::value::Obj> value = nullptr;

    if (!rhs.is_class_instant()) {
        PackedERPreliminary perp(nullptr);
        value = unpack_ER(rhs, ctx, ref, /*perp=*/&perp);
    }
    else
        value = unpack_ER(rhs, ctx, ref);

    std::shared_ptr<earl::variable::Obj> var
        = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), value, stmt->m_attrs);
    ctx->variable_add(var);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_expr(StmtExpr *stmt, std::shared_ptr<Ctx> &ctx) {
    ER er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, false);
    return unpack_ER(er, ctx, false);
}

std::shared_ptr<earl::value::Obj>
Interpreter::eval_stmt_block(StmtBlock *block, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> result = nullptr;
    ctx->push_scope();
    for (size_t i = 0; i < block->m_stmts.size(); ++i) {
        result = Interpreter::eval_stmt(block->m_stmts.at(i).get(), ctx);
        if (result && result->type() != earl::value::Type::Void)
            // We hit either a break or return statement.
            break;
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
    auto er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, false);
    auto condition = unpack_ER(er, ctx, true); // POSSIBLE BREAK, WAS FALSE
    std::shared_ptr<earl::value::Obj> result = nullptr;

    if (condition->boolean())
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);
    else if (stmt->m_else.has_value())
        result = Interpreter::eval_stmt_block(stmt->m_else.value().get(), ctx);

    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_return(StmtReturn *stmt, std::shared_ptr<Ctx> &ctx) {
    ER er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, false);
    return unpack_ER(er, ctx, false);
}

std::shared_ptr<earl::value::Obj>
eval_stmt_break(StmtBreak *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_break");
}

std::shared_ptr<earl::value::Obj>
eval_stmt_mut(StmtMut *stmt, std::shared_ptr<Ctx> &ctx) {
    ER
        left_er = Interpreter::eval_expr(stmt->m_left.get(), ctx, true), // POSSIBLE BREAK, WAS FALSE
        right_er = Interpreter::eval_expr(stmt->m_right.get(), ctx, false);
    std::shared_ptr<earl::value::Obj>
        l = unpack_ER(left_er, ctx, true),
        r = unpack_ER(right_er, ctx, false);
    l->mutate(r);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_while(StmtWhile *stmt, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj>
        expr_result = nullptr,
        result = nullptr;

    ER expr_er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, /*ref=*/false);
    expr_result = unpack_ER(expr_er, ctx, /*ref=*/true);

    while (expr_result->boolean()) {
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);

        if (result && result->type() == earl::value::Type::Break) {
            result = nullptr;
            break;
        }

        if (result && result->type() != earl::value::Type::Void)
            break;

        expr_er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, /*ref=*/false);
        expr_result = unpack_ER(expr_er, ctx, /*ref=*/true);
        if (!expr_result->boolean())
            break;
    }

    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_for(StmtFor *stmt, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> result = nullptr;
    ER start_er = Interpreter::eval_expr(stmt->m_start.get(), ctx, false);
    ER end_er = Interpreter::eval_expr(stmt->m_end.get(), ctx, false);

    auto start_expr = unpack_ER(start_er, ctx, true); // POSSIBLE BREAK, WAS FALSE
    auto end_expr = unpack_ER(end_er, ctx, true); // POSSIBLE BREAK, WAS FALSE

    auto enumerator = std::make_shared<earl::variable::Obj>(stmt->m_enumerator.get(), start_expr);

    assert(!ctx->variable_exists(enumerator->id()));
    ctx->variable_add(enumerator);

    earl::value::Int *start = dynamic_cast<earl::value::Int *>(start_expr.get());
    earl::value::Int *end = dynamic_cast<earl::value::Int *>(end_expr.get());

    while (start->value() < end->value()) {
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);

        if (result && result->type() == earl::value::Type::Break) {
            result = nullptr;
            break;
        }

        if (result && result->type() != earl::value::Type::Void)
            break;

        start->mutate(std::make_shared<earl::value::Int>(start->value()+1));
    }

    ctx->variable_remove(enumerator->id());

    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_class(StmtClass *stmt, std::shared_ptr<Ctx> &ctx) {
    dynamic_cast<WorldCtx *>(ctx.get())->define_class(stmt);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_mod(StmtMod *stmt, std::shared_ptr<Ctx> &ctx) {
    dynamic_cast<WorldCtx *>(ctx.get())->set_mod(stmt->m_id->lexeme());
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_import(StmtImport *stmt, std::shared_ptr<Ctx> &ctx) {
    assert(ctx->type() == CtxType::World);

    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types    = {};
    std::string comment               = COMMON_EARL_COMMENT;

    std::unique_ptr<Lexer> lexer      = lex_file(stmt->m_fp.get()->lexeme().c_str(), keywords, types, comment);
    std::unique_ptr<Program> program  = Parser::parse_program(*lexer.get());

    std::shared_ptr<Ctx> child_ctx = Interpreter::interpret(std::move(program), std::move(lexer));
    dynamic_cast<WorldCtx *>(ctx.get())->add_import(std::move(child_ctx));

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Interpreter::eval_stmt(Stmt *stmt, std::shared_ptr<Ctx> &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Def:       return eval_stmt_def(dynamic_cast<StmtDef *>(stmt), ctx);                  break;
    case StmtType::Let:       return eval_stmt_let(dynamic_cast<StmtLet *>(stmt), ctx);                  break;
    case StmtType::Block:     return Interpreter::eval_stmt_block(dynamic_cast<StmtBlock *>(stmt), ctx); break;
    case StmtType::Mut:       return eval_stmt_mut(dynamic_cast<StmtMut *>(stmt), ctx);                  break;
    case StmtType::Stmt_Expr: return eval_stmt_expr(dynamic_cast<StmtExpr *>(stmt), ctx);                break;
    case StmtType::If:        return eval_stmt_if(dynamic_cast<StmtIf *>(stmt), ctx);                    break;
    case StmtType::Return:    return eval_stmt_return(dynamic_cast<StmtReturn *>(stmt), ctx);            break;
    case StmtType::Break:     return eval_stmt_break(dynamic_cast<StmtBreak *>(stmt), ctx);              break;
    case StmtType::While:     return eval_stmt_while(dynamic_cast<StmtWhile *>(stmt), ctx);              break;
    case StmtType::For:       return eval_stmt_for(dynamic_cast<StmtFor *>(stmt), ctx);                  break;
    case StmtType::Import:    return eval_stmt_import(dynamic_cast<StmtImport *>(stmt), ctx);            break;
    case StmtType::Mod:       return eval_stmt_mod(dynamic_cast<StmtMod *>(stmt), ctx);                  break;
    case StmtType::Class:     return eval_stmt_class(dynamic_cast<StmtClass *>(stmt), ctx);              break;
    case StmtType::Match:     UNIMPLEMENTED("StmtType::Match");                                          break;
    default: assert(false && "unreachable");
    }
    ERR(Err::Type::Internal,
        "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong");
    return nullptr;
}

std::shared_ptr<Ctx>
Interpreter::interpret(std::unique_ptr<Program> program, std::unique_ptr<Lexer> lexer) {
    std::shared_ptr<Ctx> ctx = std::make_shared<WorldCtx>(std::move(lexer), std::move(program));
    auto wctx = dynamic_cast<WorldCtx *>(ctx.get());

    // Collect all function definitions and class definitions first...
    // Also check to make sure the first statement is a module declaration.
    // for (size_t i = 0; i < wctx->stmts_len(); ++i) {
    //     Stmt *stmt = wctx->stmt_at(i);
    //     if (i == 0 && stmt->stmt_type() != StmtType::Mod)
    //         WARN("A `mod` statement is expected to be the first statement. "
    //              "This may lead to undefined behavior and break functionality.");
    //     if (stmt->stmt_type() == StmtType::Def
    //         || stmt->stmt_type() == StmtType::Class
    //         || stmt->stmt_type() == StmtType::Mod
    //         || stmt->stmt_type() == StmtType::Import)
    //         (void)Interpreter::eval_stmt(wctx->stmt_at(i), ctx);
    // }

    for (size_t i = 0; i < wctx->stmts_len(); ++i) {
        Stmt *stmt = wctx->stmt_at(i);
        // if (stmt->stmt_type() != StmtType::Def
        //     && stmt->stmt_type() != StmtType::Class
        //     && stmt->stmt_type() == StmtType::Mod
        //     && stmt->stmt_type() == StmtType::Import)
        (void)Interpreter::eval_stmt(stmt, ctx);
    }

    return ctx;
}
