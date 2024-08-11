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
#include <variant>

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
    bool this_;
    PackedERPreliminary(std::shared_ptr<earl::value::Obj> lhs_get = nullptr, bool this_ = false)
        : lhs_getter_accessor(lhs_get), this_(this_) {}
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
                             std::shared_ptr<Ctx> &ctx,
                             bool ref) {

    bool _ref = (stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;
    std::shared_ptr<earl::value::Obj> value = nullptr;
    ER rhs = Interpreter::eval_expr(stmt->m_expr.get(), ctx, _ref);

    if (rhs.is_ident() && buffer.find(rhs.id) != buffer.end())
        value = buffer.find(rhs.id)->second->value();
    else
        value = unpack_ER(rhs, ctx, ref);

    if (!rhs.is_class_instant()) {
        PackedERPreliminary perp(nullptr);
        value = unpack_ER(rhs, ctx, _ref, /*perp=*/&perp);
    }
    else
        value = unpack_ER(rhs, ctx, _ref);

    if (stmt->m_id->lexeme() == "_")
        return nullptr;

    std::shared_ptr<earl::variable::Obj> var
        = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), value, stmt->m_attrs);
    ctx->variable_add(var);
    return nullptr;
}

static std::shared_ptr<earl::value::Obj>
eval_class_instantiation(const std::string &id,
                         std::vector<std::shared_ptr<earl::value::Obj>> &params,
                         std::shared_ptr<Ctx> &ctx,
                         bool ref) {
    (void)ref;
    StmtClass *class_stmt = nullptr;

    if (ctx->type() == CtxType::Class) {
        auto owner_ctx = dynamic_cast<WorldCtx *>(dynamic_cast<ClassCtx *>(ctx.get())->get_owner().get());
        assert(owner_ctx);
        class_stmt = owner_ctx->class_get(id);
    }
    else if (ctx->type() == CtxType::Function) {
        auto functx = dynamic_cast<FunctionCtx *>(ctx.get());
        if (functx->get_owner()->type() == CtxType::World)
            class_stmt = dynamic_cast<WorldCtx *>(functx->get_owner().get())->class_get(id);
        else {
            auto class_ctx = functx->get_outer_class_owner_ctx();

            assert(class_ctx);
            assert(class_ctx->type() == CtxType::Class);

            auto world_ctx = dynamic_cast<WorldCtx *>(dynamic_cast<ClassCtx *>(class_ctx.get())->get_owner().get());

            assert(world_ctx);
            assert(world_ctx->type() == CtxType::World);

            class_stmt = world_ctx->class_get(id);
        }
    }
    else
        class_stmt = dynamic_cast<WorldCtx *>(ctx.get())->class_get(id);

    auto class_ctx = std::make_shared<ClassCtx>(ctx);

    if (params.size() != class_stmt->m_constructor_args.size())
        ERR_WARGS(Err::Type::Fatal, "Class `%s` expects %zu arguments but %zu were supplied",
                  id.c_str(), class_stmt->m_constructor_args.size(), params.size());

    auto klass = std::make_shared<earl::value::Class>(class_stmt, class_ctx);

    // Add the constructor arguments to a temporary pushed scope
    for (size_t i = 0; i < class_stmt->m_constructor_args.size(); ++i) {
        auto var = std::make_shared<earl::variable::Obj>(class_stmt->m_constructor_args[i].get(), params[i]);

        // MAKE SURE TO CLEAR AT THE END OF THIS FUNC!
        class_ctx->fill___m_class_constructor_tmp_args(var);
    }

    // Eval member variables
    for (auto &member : class_stmt->m_members)
        (void)eval_stmt_let_wcustom_buffer(member.get(),
                                           class_ctx->get___m_class_constructor_tmp_args(),
                                           klass->ctx(),
                                           true);

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

    // CLEARED!
    class_ctx->clear___m_class_constructor_tmp_args();

    return klass;
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

static std::vector<std::shared_ptr<earl::value::Obj>>
evaluate_function_parameters_wrefs(ExprFuncCall *funccall,
                                   std::variant<std::shared_ptr<earl::function::Obj>, earl::value::Closure *> &func_proper,
                                   std::shared_ptr<Ctx> ctx) {
    std::vector<std::shared_ptr<earl::value::Obj>> res = {};
    std::vector<int> refs = {};

    std::visit([&](auto &&fun) {
        using T = std::decay_t<decltype(fun)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<earl::function::Obj>>) {

            // Build reference table
            for (size_t i = 0; i < fun->params_len(); ++i)
                refs.push_back(static_cast<bool>(fun->param_at_is_ref(i)));

            // Evaluate the parameters based on the reference table
            for (size_t i = 0; i < funccall->m_params.size(); ++i) {
                ER er = Interpreter::eval_expr(funccall->m_params[i].get(), ctx, /*ref=*/refs[i]);
                res.push_back(unpack_ER(er, ctx, refs[i]));
                // if (refs[i]) res.push_back(unpack_ER(er, ctx, true));
                // else         res.push_back(unpack_ER(er, ctx, false));
            }
        }

        else if constexpr (std::is_same_v<T, earl::value::Closure *>) {

            // Build reference table
            for (size_t i = 0; i < fun->params_len(); ++i)
                refs.push_back(static_cast<bool>(fun->param_at_is_ref(i)));

            // Evaluate the parameters based on the reference table
            for (size_t i = 0; i < funccall->m_params.size(); ++i) {
                ER er = Interpreter::eval_expr(funccall->m_params[i].get(), ctx, /*ref=*/refs[i]);
                res.push_back(unpack_ER(er, ctx, refs[i]));
                // if (refs[i]) res.push_back(unpack_ER(er, ctx, true));
                // else         res.push_back(unpack_ER(er, ctx, false));
            }
        }

        else
            ERR(Err::Type::Internal,
                "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong");
    }, func_proper);

    return res;
}

static std::shared_ptr<earl::value::Obj>
eval_user_defined_function_wo_params(const std::string &id,
                                     ExprFuncCall *funccall,
                                     std::shared_ptr<Ctx> &funccall_ctx,
                                     std::shared_ptr<Ctx> &ctx,
                                     bool from_outside = false) {
    std::vector<std::shared_ptr<earl::value::Obj>> params = {};
    std::vector<int> refs = {};
    std::variant<std::shared_ptr<earl::function::Obj>, earl::value::Closure *> v;

    if (ctx->function_exists(id)) {
        auto func = ctx->function_get(id);
        v = func;
        if (from_outside && !func->is_pub())
            ERR_WARGS(Err::Type::Fatal, "function `%s` does not contain the @pub attribute", id.c_str());

        params = evaluate_function_parameters_wrefs(funccall, v, funccall_ctx);

        auto fctx = std::make_shared<FunctionCtx>(ctx, func->attrs());
        func->load_parameters(params, fctx);
        std::shared_ptr<Ctx> mask = fctx;
        return Interpreter::eval_stmt_block(func->block(), mask);
    }
    else if (ctx->closure_exists(id)) {
        auto cl = ctx->variable_get(id);
        auto clctx = std::make_shared<ClosureCtx>(ctx);
        earl::value::Closure *clvalue = nullptr;

        clvalue = dynamic_cast<earl::value::Closure *>(cl->value().get());

        v = clvalue;
        params = evaluate_function_parameters_wrefs(funccall, v, funccall_ctx);
        clvalue->load_parameters(params, clctx);
        std::shared_ptr<Ctx> mask = clctx;
        return Interpreter::eval_stmt_block(clvalue->block(), mask);
    }

    ERR_WARGS(Err::Type::Undeclared, "function `%s` has not been defined", id.c_str());
    return nullptr; // unreachable
}

static std::shared_ptr<earl::value::Obj>
eval_user_defined_function(const std::string &id,
                           std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           bool from_outside) {
    if (ctx->function_exists(id)) {
        auto func = ctx->function_get(id);
        if (from_outside && !func->is_pub())
            ERR_WARGS(Err::Type::Fatal, "function `%s` does not contain the @pub attribute", id.c_str());
        auto fctx = std::make_shared<FunctionCtx>(ctx, func->attrs());
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
            if (!Intrinsics::is_member_intrinsic(er.id, static_cast<int>(perp->lhs_getter_accessor->type()))) {
                ERR_WARGS(Err::Type::Fatal, "type `%s` does not implement member intrinsic `%s`",
                          earl::value::type_to_str(perp->lhs_getter_accessor->type()).c_str(), er.id.c_str());
            }
            return Intrinsics::call_member(er.id,
                                           perp->lhs_getter_accessor->type(),
                                           perp->lhs_getter_accessor,
                                           params,
                                           ctx);
        }

        if (ctx->type() == CtxType::Class) {
            std::shared_ptr<earl::function::Obj> func = nullptr;
            if (ctx->function_exists(er.id))
                func = ctx->function_get(er.id);
            if ((!perp || !perp->this_) && (func && er.ctx != ctx && !func->is_pub()))
                ERR_WARGS(Err::Type::Fatal, "member variable `%s` is missing the @pub attribute", func->id().c_str());
            return eval_user_defined_function(er.id, params, ctx);
        }

        // We need to have this function to gen the parameters so we
        // know which ones need to be taken as a reference. NOTE: The
        // routine(s) above this may need this change as well.
        return eval_user_defined_function_wo_params(er.id, static_cast<ExprFuncCall *>(er.extra), er.ctx, ctx);
    }
    else if (er.is_literal())
        return er.value;
    else if (er.is_ident()) {
        if (ctx->variable_exists(er.id)) {
            auto var = ctx->variable_get(er.id);
            if ((!perp || !perp->this_) && (er.ctx != ctx && !var->is_pub()))
                ERR_WARGS(Err::Type::Fatal, "member variable `%s` is missing the @pub attribute", var->id().c_str());
            if (!ref)
                return var->value()->copy();
            return var->value();
        }
        // Check if it is an enum
        WorldCtx *world = ctx->type() == CtxType::World ?
            dynamic_cast<WorldCtx *>(ctx.get()) :
            ctx->get_world();
        if (world->enum_exists(er.id))
            return world->enum_get(er.id);
        // Check if it is an entry of an enum
        if (perp && perp->lhs_getter_accessor && perp->lhs_getter_accessor->type() == earl::value::Type::Enum) {
            auto lhs = dynamic_cast<earl::value::Enum *>(perp->lhs_getter_accessor.get());
            if (lhs->has_entry(er.id))
                return lhs->get_entry(er.id)->value()->copy();
        }
        ERR_WARGS(Err::Type::Fatal, "variable `%s` has not been declared", er.id.c_str());
    }
    else if (er.is_wildcard())
        return std::make_shared<earl::value::Void>();
    else
        assert(false);
}

static ER
eval_expr_term_ident(ExprIdent *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    (void)ref;
    const std::string &id = expr->m_tok->lexeme();
    if (id == "_")
        return ER(nullptr, ERT::Wildcard, /*id=*/id, /*extra=*/nullptr, /*ctx=*/ctx);
    return ER(nullptr, ERT::Ident, /*id=*/id, /*extra=*/nullptr, /*ctx=*/ctx);
}

// RETURNS ACTUAL EVALUATED VALUE IN ER
static ER
eval_expr_term_intlit(ExprIntLit *expr) {
    auto value = std::make_shared<earl::value::Int>(std::stoi(expr->m_tok->lexeme()));
    return ER(value, ERT::Literal);
}

// RETURNS ACTUAL EVALUATED VALUE IN ER
static ER
eval_expr_term_strlit(ExprStrLit *expr) {
    auto value = std::make_shared<earl::value::Str>(expr->m_tok->lexeme());
    return ER(value, ERT::Literal);
}

static ER
eval_expr_term_funccall(ExprFuncCall *expr, std::shared_ptr<Ctx> &ctx, bool ref) {

    // Checks if `_id` is a class in the @world scope.
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
    if (ctx_wclass)
        return ER(nullptr, static_cast<ERT>(ERT::ClassInstant|ERT::Literal), /*id=*/id, /*extra=*/static_cast<void *>(expr), /*ctx=*/ctx_wclass);

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

    if (ctx->type() == CtxType::World)
        ctx_ptr = dynamic_cast<WorldCtx *>(ctx.get())->get_import(left_id);
    else if (ctx->type() == CtxType::Function) {
        auto world = dynamic_cast<FunctionCtx *>(ctx.get())->get_outer_world_owner();
        ctx_ptr = dynamic_cast<WorldCtx *>(world.get())->get_import(left_id);
    }
    else if (ctx->type() == CtxType::Class)
        UNIMPLEMENTED("eval_expr_term_mod_access:ctx->type() == CtxType::Class");
    else if (ctx->type() == CtxType::Closure) {
        auto world = dynamic_cast<ClosureCtx *>(ctx.get())->get_outer_world_owner();
        ctx_ptr = dynamic_cast<WorldCtx *>(world.get())->get_import(left_id);
    }

    std::shared_ptr<Ctx> &other_ctx = *ctx_ptr;

    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            right_er = Interpreter::eval_expr(arg.get(), other_ctx, true);
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            right_er = Interpreter::eval_expr(arg.get(), other_ctx, true);
        else
            ERR(Err::Type::Internal,
                "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong");
    }, expr->m_right);

    if (right_er.is_class_instant()) {
        assert(right_er.ctx->type() == CtxType::World);
        WorldCtx *world = dynamic_cast<WorldCtx *>(right_er.ctx.get());
        if (world->class_is_defined(right_er.id)) {
            if ((world->class_get(right_er.id)->m_attrs & static_cast<uint32_t>(Attr::Pub)) == 0) {
                ERR_WARGS(Err::Type::Fatal, "class `%s` in module `%s` does not contain the @pub attribute",
                        right_er.id.c_str(), left_id.c_str());
            }
        }
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(right_er.extra), ctx, ref);
        auto class_instantiation = eval_class_instantiation(right_er.id, params, right_er.ctx, ref);
        return ER(class_instantiation, ERT::Literal, /*id=*/"", /*extra=*/nullptr, /*ctx=*/ctx);
    }
    if (right_er.is_function_ident()) {
        // Check if the function has @pub attribute
        if (right_er.ctx->function_exists(right_er.id)) {
            if (!right_er.ctx->function_get(right_er.id)->is_pub())
                ERR_WARGS(Err::Type::Fatal, "function `%s` in module `%s` does not contain the @pub attribute",
                        right_er.id.c_str(), left_id.c_str());
        }
        // Check if the closure has @pub attribute
        else if (right_er.ctx->closure_exists(right_er.id)) {
            if (!right_er.ctx->function_get(right_er.id)->is_pub())
                ERR_WARGS(Err::Type::Fatal, "variable `%s` in module `%s` does not contain the @pub attribute",
                        right_er.id.c_str(), left_id.c_str());
        }
        auto func = eval_user_defined_function_wo_params(right_er.id, static_cast<ExprFuncCall *>(right_er.extra), ctx, right_er.ctx);
        return ER(func, ERT::Literal);
    }
    else if (right_er.is_ident()) {
        auto value = unpack_ER(right_er, other_ctx, ref);
        // Check if it is a variable
        if (right_er.ctx->variable_exists(right_er.id)) {
            if (!right_er.ctx->variable_get(right_er.id)->is_pub())
                ERR_WARGS(Err::Type::Fatal, "variable `%s` in module `%s` does not contain the @pub attribute",
                          right_er.id.c_str(), left_id.c_str());
            return ER(value, ERT::Literal);
        }
        // It must be an enum
        assert(value->type() == earl::value::Type::Enum);
        if (!dynamic_cast<earl::value::Enum *>(value.get())->is_pub())
            ERR_WARGS(Err::Type::Fatal, "enumeration `%s` in module `%s` does not contain the @pub attribute",
                      right_er.id.c_str(), left_id.c_str());
        return ER(value, ERT::Literal);
    }
    else
        assert(false && "unimplemented");
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

        PackedERPreliminary perp(nullptr, true);
        auto value = unpack_ER(right_er, fctx->get_outer_class_owner_ctx(), /*ref=*/true, /*perp=*/&perp);
        return ER(value, ERT::Literal);
    }
    else {
        auto left_value = unpack_ER(left_er, ctx, true);
        PackedERPreliminary perp(left_value);
        std::shared_ptr<earl::value::Obj> value = nullptr;

        if (left_value->type() == earl::value::Type::Class) {
            // Class method/member. The right side (right_er) contains the actual call/identifier to be evaluated,
            // and we need the left (left_value)'s context with the preliminary value of (perp).
            value = unpack_ER(right_er, dynamic_cast<earl::value::Class *>(left_value.get())->ctx(), ref, &perp);
        }
        else
            // Function chaining and member intrinsics...
            value = unpack_ER(right_er, ctx, ref, &perp);

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
    else if (left_value->type() == earl::value::Type::Tuple) {
        auto tuple = dynamic_cast<earl::value::Tuple *>(left_value.get());
        return ER(tuple->nth(idx_value), static_cast<ERT>(ERT::Literal|ERT::TupleAccess));
    }
    else
        ERR(Err::Type::Fatal, "cannot use `[]` on non-list, non-tuple, or non-str type");
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
    (void)ref;
    std::vector<std::pair<Token *, uint32_t>> args;
    for (auto &entry : expr->m_args)
        args.push_back(std::make_pair(entry.first.get(), entry.second));
    auto cl = std::make_shared<earl::value::Closure>(expr, std::move(args), ctx);
    return ER(cl, ERT::Literal);
}

static ER
eval_expr_term_floatlit(ExprFloatLit *expr) {
    auto value = std::make_shared<earl::value::Float>(std::stof(expr->m_tok->lexeme()));
    return ER(value, ERT::Literal);
}

static ER
eval_expr_term_range(ExprRange *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    ER left_er = Interpreter::eval_expr(expr->m_start.get(), ctx, ref);
    ER right_er = Interpreter::eval_expr(expr->m_end.get(), ctx, ref);
    auto lvalue = unpack_ER(left_er, ctx, ref);
    auto rvalue = unpack_ER(right_er, ctx, ref);

    if (lvalue->type() != rvalue->type())
        ERR(Err::Type::Fatal, "type mismatch for generating a range");

    std::vector<std::shared_ptr<earl::value::Obj>> values = {};

    switch (lvalue->type()) {
    case earl::value::Type::Int: {
        int start = dynamic_cast<earl::value::Int *>(lvalue.get())->value();
        int end = dynamic_cast<earl::value::Int *>(rvalue.get())->value();
        while (start < end)
            values.push_back(std::make_shared<earl::value::Int>(start++));
        return ER(std::make_shared<earl::value::List>(values), ERT::Literal);
    } break;
    case earl::value::Type::Char: {
        char start = dynamic_cast<earl::value::Char *>(lvalue.get())->value();
        char end = dynamic_cast<earl::value::Char *>(rvalue.get())->value();
        while (start < end)
            values.push_back(std::make_shared<earl::value::Char>(std::string(1, start++)));
        return ER(std::make_shared<earl::value::List>(values), ERT::Literal);
    }
    default: {
        ERR_WARGS(Err::Type::Fatal, "invalid type `%s` for type range", earl::value::type_to_str(lvalue->type()).c_str());
    } break;
    }
}

static ER
eval_expr_term_tuple(ExprTuple *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    std::vector<std::shared_ptr<earl::value::Obj>> values = {};
    for (auto &e : expr->m_exprs) {
        ER er = Interpreter::eval_expr(e.get(), ctx, ref);
        auto value = unpack_ER(er, ctx, ref);
        values.push_back(value);
    }
    return ER(std::make_shared<earl::value::Tuple>(values), ERT::Literal);
}

ER
eval_expr_term(ExprTerm *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident:         return eval_expr_term_ident(dynamic_cast<ExprIdent *>(expr), ctx, ref);
    case ExprTermType::Int_Literal:   return eval_expr_term_intlit(dynamic_cast<ExprIntLit *>(expr));
    case ExprTermType::Str_Literal:   return eval_expr_term_strlit(dynamic_cast<ExprStrLit *>(expr));
    case ExprTermType::Char_Literal:  return eval_expr_term_charlit(dynamic_cast<ExprCharLit *>(expr));
    case ExprTermType::Float_Literal: return eval_expr_term_floatlit(dynamic_cast<ExprFloatLit *>(expr));
    case ExprTermType::Func_Call:     return eval_expr_term_funccall(dynamic_cast<ExprFuncCall *>(expr), ctx, ref);
    case ExprTermType::List_Literal:  return eval_expr_term_listlit(dynamic_cast<ExprListLit *>(expr), ctx, ref);
    case ExprTermType::Get:           return eval_expr_term_get(dynamic_cast<ExprGet *>(expr), ctx, ref);
    case ExprTermType::Mod_Access:    return eval_expr_term_mod_access(dynamic_cast<ExprModAccess *>(expr), ctx, ref);
    case ExprTermType::Array_Access:  return eval_expr_term_array_access(dynamic_cast<ExprArrayAccess *>(expr), ctx, ref);
    case ExprTermType::Bool:          return eval_expr_term_boollit(dynamic_cast<ExprBool *>(expr));
    case ExprTermType::None:          return eval_expr_term_none(dynamic_cast<ExprNone *>(expr));
    case ExprTermType::Closure:       return eval_expr_term_closure(dynamic_cast<ExprClosure *>(expr), ctx, ref);
    case ExprTermType::Range:         return eval_expr_term_range(dynamic_cast<ExprRange *>(expr), ctx, ref);
    case ExprTermType::Tuple:         return eval_expr_term_tuple(dynamic_cast<ExprTuple *>(expr), ctx, ref);
    default:                          ERR_WARGS(Err::Type::Fatal, "unknown term: `%d`", (int)expr->get_term_type());
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
eval_expr_unary(ExprUnary *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    ER rhs = Interpreter::eval_expr(expr->m_expr.get(), ctx, ref);
    auto expr_value = unpack_ER(rhs, ctx, ref);
    auto result = expr_value->unaryop(expr->m_op.get());
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
    case ExprType::Unary: {
        return eval_expr_unary(dynamic_cast<ExprUnary *>(expr), ctx, ref);
    } break;
    default:
        assert(false && "unreachable");
    }
}

std::shared_ptr<earl::value::Obj>
eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx) {
    if (ctx->type() == CtxType::Closure)
        // Special case for when we declare a variable in a recursive closure.
        dynamic_cast<ClosureCtx *>(ctx.get())->assert_variable_does_not_exist_for_recursive_cl(stmt->m_id->lexeme());
    else {
        if (ctx->variable_exists(stmt->m_id->lexeme())) {
            Err::err_wtok(stmt->m_id.get());
            ERR_WARGS(Err::Type::Redeclared, "variable `%s` is already declared", stmt->m_id->lexeme().c_str());
        }
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

    if (stmt->m_id->lexeme() == "_")
        return nullptr;

    std::shared_ptr<earl::variable::Obj> var
        = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), value, stmt->m_attrs);
    ctx->variable_add(var);
    return nullptr;
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
    if (stmt->m_expr.has_value()) {
        ER er = Interpreter::eval_expr(stmt->m_expr.value().get(), ctx, false);
        return unpack_ER(er, ctx, false);
    }
    return std::make_shared<earl::value::Option>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_break(StmtBreak *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    return std::make_shared<earl::value::Break>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_mut(StmtMut *stmt, std::shared_ptr<Ctx> &ctx) {
    ER left_er = Interpreter::eval_expr(stmt->m_left.get(), ctx, true);
    ER right_er = Interpreter::eval_expr(stmt->m_right.get(), ctx, false);

    if (left_er.is_tuple_access())
        ERR(Err::Type::Fatal, "cannot mutate tuple type as they are immutable");

    auto l = unpack_ER(left_er, ctx, true);
    auto r = unpack_ER(right_er, ctx, false);
    switch (stmt->m_equals->type()) {
    case TokenType::Equals: {
        l->mutate(r);
    } break;
    case TokenType::Plus_Equals:
    case TokenType::Minus_Equals:
    case TokenType::Asterisk_Equals:
    case TokenType::Forwardslash_Equals: {
        l->spec_mutate(stmt->m_equals.get(), r);
    } break;
    default: {
        Err::err_wtok(stmt->m_equals.get());
        ERR_WARGS(Err::Type::Fatal, "invalid mutation operation `%s`", stmt->m_equals->lexeme().c_str());
    } break;
    }
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
eval_stmt_foreach(StmtForeach *stmt, std::shared_ptr<Ctx> &ctx) {
    bool ref = (stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;

    std::shared_ptr<earl::value::Obj> result = nullptr;
    ER expr_er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, ref);
    auto expr = unpack_ER(expr_er, ctx, ref);

    if (expr->type() == earl::value::Type::List) {
        auto lst = std::dynamic_pointer_cast<earl::value::List>(expr);
        if (lst->value().size() == 0)
            return result;
        auto enumerator = std::make_shared<earl::variable::Obj>(stmt->m_enumerator.get(), lst->value()[0]);
        if (ctx->variable_exists(enumerator->id())) {
            Err::err_wtok(stmt->m_enumerator.get());
            ERR_WARGS(Err::Type::Redeclared, "variable `%s` is already declared", stmt->m_enumerator->lexeme().c_str());
        }
        ctx->variable_add(enumerator);
        for (size_t i = 0; i < lst->value().size(); ++i) {
            if (i != 0)
                enumerator->reset(lst->value()[i]);
            result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);
            if (result && result->type() == earl::value::Type::Break) {
                result = nullptr;
                break;
            }
            if (result && result->type() != earl::value::Type::Void)
                break;
        }
        ctx->variable_remove(enumerator->id());
    }
    else if (expr->type() == earl::value::Type::Tuple) {
        auto tuple = std::dynamic_pointer_cast<earl::value::Tuple>(expr);
        if (tuple->value().size() == 0)
            return result;
        auto enumerator = std::make_shared<earl::variable::Obj>(stmt->m_enumerator.get(), tuple->value()[0]);
        if (ctx->variable_exists(enumerator->id())) {
            Err::err_wtok(stmt->m_enumerator.get());
            ERR_WARGS(Err::Type::Redeclared, "variable `%s` is already declared", stmt->m_enumerator->lexeme().c_str());
        }
        ctx->variable_add(enumerator);
        for (size_t i = 0; i < tuple->value().size(); ++i) {
            if (i != 0)
                enumerator->reset(tuple->value()[i]);
            result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);
            if (result && result->type() == earl::value::Type::Break) {
                result = nullptr;
                break;
            }
            if (result && result->type() != earl::value::Type::Void)
                break;
        }
        ctx->variable_remove(enumerator->id());
    }
    else if (expr->type() == earl::value::Type::Str) {
        auto str = std::dynamic_pointer_cast<earl::value::Str>(expr);
        if (str->value().size() == 0)
            return result;
        auto enumerator = std::make_shared<earl::variable::Obj>(stmt->m_enumerator.get(), str->value_raw()[0]);
        if (ctx->variable_exists(enumerator->id())) {
            Err::err_wtok(stmt->m_enumerator.get());
            ERR_WARGS(Err::Type::Redeclared, "variable `%s` is already declared", stmt->m_enumerator->lexeme().c_str());
        }
        ctx->variable_add(enumerator);
        for (size_t i = 0; i < str->value().size(); ++i) {
            if (i != 0)
                enumerator->reset(str->value_raw()[i]);
            result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);
            if (result && result->type() == earl::value::Type::Break) {
                result = nullptr;
                break;
            }
            if (result && result->type() != earl::value::Type::Void)
                break;
        }
    }
    else
        ERR(Err::Type::Fatal, "unable to perform a `for` loop with an expression other than a list, str, or tuple type");

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

    bool lt = start->value() <= end->value();
    bool gt = start->value() > end->value();

    while (true) {
        if (lt && start->value() > end->value()-1)
            break;
        else if (gt && start->value() < end->value())
            break;

        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);

        if (result && result->type() == earl::value::Type::Break) {
            result = nullptr;
            break;
        }

        if (result && result->type() != earl::value::Type::Void)
            break;

        if (lt)
            start->mutate(std::make_shared<earl::value::Int>(start->value()+1));
        else if (gt)
            start->mutate(std::make_shared<earl::value::Int>(start->value()-1));
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
    assert(child_ctx->type() == CtxType::World);
    if (stmt->__m_depth == COMMON_DEPTH_ALMOST)
        dynamic_cast<WorldCtx *>(child_ctx.get())->strip_funs_and_classes();
    dynamic_cast<WorldCtx *>(ctx.get())->add_import(std::move(child_ctx));
    return std::make_shared<earl::value::Void>();
}

static std::shared_ptr<earl::variable::Obj>
handle_match_some_branch(ExprFuncCall *expr, std::shared_ptr<earl::value::Obj> inject_value, std::shared_ptr<Ctx> &ctx) {
    assert(expr->m_params.size() == 1);

    Expr *value = expr->m_params[0].get();
    if (value->get_type() != ExprType::Term)
        return nullptr;

    auto *term = dynamic_cast<ExprTerm *>(value);
    if (term->get_term_type() != ExprTermType::Ident)
        return nullptr;

    auto *ident = dynamic_cast<ExprIdent *>(term);

    if (ctx->variable_exists(ident->m_tok->lexeme())) {
        Err::err_wtok(ident->m_tok.get());
        ERR_WARGS(Err::Type::Redeclared, "variable %s in match statement is already declared",
                  ident->m_tok->lexeme().c_str());
    }

    auto unwrapped_value = dynamic_cast<earl::value::Option *>(inject_value.get())->value()->copy();
    auto var = std::make_shared<earl::variable::Obj>(ident->m_tok.get(), unwrapped_value, 0);

    return var;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_match(StmtMatch *stmt, std::shared_ptr<Ctx> &ctx) {
    ER match_er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, true);
    auto match_value = unpack_ER(match_er, ctx, true);

    // Go through the branches
    for (size_t i = 0; i < stmt->m_branches.size(); ++i) {
        StmtMatch::Branch *branch = stmt->m_branches[i].get();

        // Go through the different expressions that are separated by `|`
        for (size_t j = 0; j < branch->m_expr.size(); ++j) {
            std::shared_ptr<earl::value::Obj>
                potential_match = nullptr,
                guard = nullptr;

            if (branch->m_expr[j]->get_type() == ExprType::Term &&
                (dynamic_cast<ExprTerm *>(branch->m_expr[j].get())->get_term_type() == ExprTermType::Func_Call)) {

                auto test2 = dynamic_cast<ExprFuncCall *>(branch->m_expr[j].get());
                ER possible_id = Interpreter::eval_expr(test2->m_left.get(), ctx, true);
                const std::string &id = possible_id.id;
                if (id == "some" && match_value->type() == earl::value::Type::Option) {
                    auto tmp_var = handle_match_some_branch(test2, match_value, ctx);

                    if (tmp_var) {
                        ctx->variable_add(tmp_var);

                        if (branch->m_when.has_value()) {
                            ER _guard = Interpreter::eval_expr(branch->m_when.value().get(), ctx, true);
                            guard = unpack_ER(_guard, ctx, true);
                        }

                        if (guard == nullptr || guard->boolean()) {
                            auto res = Interpreter::eval_stmt_block(branch->m_block.get(), ctx);
                            ctx->variable_remove(tmp_var->id());
                            return res;
                        }
                        else
                            ctx->variable_remove(tmp_var->id());
                    }
                    else
                        // It is `some`, but it does not have a variable
                        goto not_some;
                }
                else
                    // It is not `some`
                    goto not_some;
            }
            else {
            not_some:
                ER _potential_match = Interpreter::eval_expr(branch->m_expr[j].get(), ctx, true);
                potential_match = unpack_ER(_potential_match, ctx, true);
                if (match_value->eq(potential_match) || potential_match->type() == earl::value::Type::Void) {
                    if (branch->m_when.has_value()) {
                        ER _guard = Interpreter::eval_expr(branch->m_when.value().get(), ctx, true);
                        guard = unpack_ER(_guard, ctx, true);
                    }
                    if (guard == nullptr || guard->boolean()) {
                        return Interpreter::eval_stmt_block(branch->m_block.get(), ctx);
                    }
                }
            }
        }
    }

    return nullptr;
}

static std::shared_ptr<earl::value::Obj>
eval_stmt_enum(StmtEnum *stmt, std::shared_ptr<Ctx> &ctx) {
    std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> elems = {};

    bool mixed_types = false;
    bool found_unassigned = false;

    earl::value::Int *last_value = nullptr;
    for (auto &p : stmt->m_elems) {
        std::shared_ptr<earl::variable::Obj> var = nullptr;
        if (p.second) {
            ER er = Interpreter::eval_expr(p.second.get(), ctx, false);
            auto value = unpack_ER(er, ctx, false);
            if (value->type() != earl::value::Type::Int)
                mixed_types = true;
            var = std::make_shared<earl::variable::Obj>(p.first.get(), value);
            last_value = dynamic_cast<earl::value::Int *>(value.get());
        }
        else {
            found_unassigned = true;
            if (mixed_types)
                ERR(Err::Type::Fatal, "if using datatypes other than integers inside of an enum, all entries must be explicitly assigned");
            int actual = 0;
            if (last_value)
                actual = last_value->value()+1;
            auto value = std::make_shared<earl::value::Int>(actual);
            last_value = value.get();
            var = std::make_shared<earl::variable::Obj>(p.first.get(), std::shared_ptr<earl::value::Obj>(value));
        }
        elems.insert({p.first->lexeme(), std::move(var)});
    }

    if (mixed_types && found_unassigned)
        ERR(Err::Type::Fatal, "if using datatypes other than integers inside of an enum, all entries must be explicitly assigned");

    auto _enum = std::make_shared<earl::value::Enum>(stmt, std::move(elems), stmt->m_attrs);
    assert(ctx->type() == CtxType::World);
    dynamic_cast<WorldCtx *>(ctx.get())->enum_add(std::move(_enum));
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Interpreter::eval_stmt(Stmt *stmt, std::shared_ptr<Ctx> &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Def:       return eval_stmt_def(dynamic_cast<StmtDef *>(stmt), ctx);
    case StmtType::Let:       return eval_stmt_let(dynamic_cast<StmtLet *>(stmt), ctx);
    case StmtType::Block:     return Interpreter::eval_stmt_block(dynamic_cast<StmtBlock *>(stmt), ctx);
    case StmtType::Mut:       return eval_stmt_mut(dynamic_cast<StmtMut *>(stmt), ctx);
    case StmtType::Stmt_Expr: return eval_stmt_expr(dynamic_cast<StmtExpr *>(stmt), ctx);
    case StmtType::If:        return eval_stmt_if(dynamic_cast<StmtIf *>(stmt), ctx);
    case StmtType::Return:    return eval_stmt_return(dynamic_cast<StmtReturn *>(stmt), ctx);
    case StmtType::Break:     return eval_stmt_break(dynamic_cast<StmtBreak *>(stmt), ctx);
    case StmtType::While:     return eval_stmt_while(dynamic_cast<StmtWhile *>(stmt), ctx);
    case StmtType::Foreach:   return eval_stmt_foreach(dynamic_cast<StmtForeach *>(stmt), ctx);
    case StmtType::For:       return eval_stmt_for(dynamic_cast<StmtFor *>(stmt), ctx);
    case StmtType::Import:    return eval_stmt_import(dynamic_cast<StmtImport *>(stmt), ctx);
    case StmtType::Mod:       return eval_stmt_mod(dynamic_cast<StmtMod *>(stmt), ctx);
    case StmtType::Class:     return eval_stmt_class(dynamic_cast<StmtClass *>(stmt), ctx);
    case StmtType::Match:     return eval_stmt_match(dynamic_cast<StmtMatch *>(stmt), ctx);
    case StmtType::Enum:      return eval_stmt_enum(dynamic_cast<StmtEnum *>(stmt), ctx);
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
    for (size_t i = 0; i < wctx->stmts_len(); ++i) {
        Stmt *stmt = wctx->stmt_at(i);
        if (i == 0 && stmt->stmt_type() != StmtType::Mod)
            WARN("A `mod` statement is expected to be the first statement. "
                 "This may lead to undefined behavior and break functionality.");
        if (stmt->stmt_type() == StmtType::Def
            || stmt->stmt_type() == StmtType::Class
            || stmt->stmt_type() == StmtType::Mod
            || stmt->stmt_type() == StmtType::Import)
            (void)Interpreter::eval_stmt(wctx->stmt_at(i), ctx);
    }

    for (size_t i = 0; i < wctx->stmts_len(); ++i) {
        Stmt *stmt = wctx->stmt_at(i);
        if (stmt->stmt_type() != StmtType::Def
            && stmt->stmt_type() != StmtType::Class
            && stmt->stmt_type() != StmtType::Mod
            && stmt->stmt_type() != StmtType::Import)
            (void)Interpreter::eval_stmt(stmt, ctx);
    }

    return ctx;
}
