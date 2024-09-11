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
    Token *errtok;
    PackedERPreliminary(std::shared_ptr<earl::value::Obj> lhs_get = nullptr,
                        bool this_ = false,
                        Token *errtok = nullptr)
        : lhs_getter_accessor(lhs_get), this_(this_), errtok(errtok) {}
};

static std::shared_ptr<earl::value::Obj>
eval_user_defined_function(ExprFuncCall *expr,
                           const std::string &id,
                           std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           bool from_outside = false);

std::shared_ptr<earl::value::Obj>
eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx);

std::shared_ptr<earl::value::Obj>
eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx);

static std::shared_ptr<earl::value::Obj>
unpack_ER(ER &er, std::shared_ptr<Ctx> &ctx, bool ref, PackedERPreliminary *perp = nullptr);

static std::string
identifier_not_declared(std::string given, std::vector<std::string> possible, bool include_intrinsics=true) {
    if (include_intrinsics) {
        for (auto it = Intrinsics::intrinsic_functions.begin(); it != Intrinsics::intrinsic_functions.end(); ++it)
            possible.push_back(it->first);
    }

    std::vector<int> ranks = {};
    int min = 1e9;
    int min_idx = 0;

    for (size_t i = 0; i < possible.size(); ++i) {
        int rank = levenshtein_distance(given, possible[i]);
        if (rank < min) {
            min = rank;
            min_idx = i;
        }
    }

    return possible.at(min_idx);
}

static std::string
method_not_declared(std::string given, std::shared_ptr<earl::value::Obj> accessor) {
    std::vector<std::string> possible = {};
    switch (accessor->type()) {
    case earl::value::Type::List: {
        for (auto it = Intrinsics::intrinsic_list_member_functions.begin(); it != Intrinsics::intrinsic_list_member_functions.end(); ++it)
            possible.push_back(it->first);
    } break;
    case earl::value::Type::Str: {
        for (auto it = Intrinsics::intrinsic_str_member_functions.begin(); it != Intrinsics::intrinsic_str_member_functions.end(); ++it)
            possible.push_back(it->first);
    } break;
    case earl::value::Type::Char: {
        for (auto it = Intrinsics::intrinsic_char_member_functions.begin(); it != Intrinsics::intrinsic_char_member_functions.end(); ++it)
            possible.push_back(it->first);
    } break;
    case earl::value::Type::Option: {
        for (auto it = Intrinsics::intrinsic_option_member_functions.begin(); it != Intrinsics::intrinsic_option_member_functions.end(); ++it)
            possible.push_back(it->first);
    } break;
    case earl::value::Type::File: {
        for (auto it = Intrinsics::intrinsic_file_member_functions.begin(); it != Intrinsics::intrinsic_file_member_functions.end(); ++it)
            possible.push_back(it->first);
    } break;
    case earl::value::Type::Tuple: {
        for (auto it = Intrinsics::intrinsic_tuple_member_functions.begin(); it != Intrinsics::intrinsic_tuple_member_functions.end(); ++it)
            possible.push_back(it->first);
    } break;
    case earl::value::Type::DictInt:
    case earl::value::Type::DictStr:
    case earl::value::Type::DictFloat:
    case earl::value::Type::DictChar: {
        for (auto it = Intrinsics::intrinsic_dict_member_functions.begin(); it != Intrinsics::intrinsic_dict_member_functions.end(); ++it)
            possible.push_back(it->first);
    } break;
    default: {
        return identifier_not_declared(given, possible);
    } break;
    }

    return identifier_not_declared(given, possible, /*include_intrinsics=*/false);
}

static std::shared_ptr<earl::value::Obj>
eval_stmt_let_wmultiple_vars_wcustom_buffer_in_class(StmtLet *stmt,
                                             std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> &buffer,
                                             std::shared_ptr<Ctx> &ctx,
                                             bool ref) {
    bool _ref = (stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;
    bool _const = (stmt->m_attrs & static_cast<uint32_t>(Attr::Const)) != 0;
    std::shared_ptr<earl::value::Obj> value = nullptr;
    ER rhs = Interpreter::eval_expr(stmt->m_expr.get(), ctx, _ref);

    {
        int i = 0;
        for (auto &id : stmt->m_ids) {
            if (dynamic_cast<ClassCtx *>(ctx.get())->variable_exists_wo__m_class_constructor_tmp_args(id->lexeme())) {
                std::string msg = "variable `"+id->lexeme()+"` is already declared";
                auto conflict = ctx->variable_get(id->lexeme());
                Err::err_wconflict(stmt->m_ids.at(i).get(), conflict->gettok());
                throw InterpreterException(msg);
                ++i;
            }
        }
    }

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

    if (value->type() != earl::value::Type::Tuple) {
        Err::err_wexpr(stmt->m_expr.get());
        const std::string msg = "cannot declare multiple variables that do not equate to a tuple expression";
        throw InterpreterException(msg);
    }

    auto tuple = dynamic_cast<earl::value::Tuple *>(value.get());

    if (tuple->value().size() != stmt->m_ids.size()) {
        Err::err_wexpr(stmt->m_expr.get());
        const std::string msg =
            "the number of variables declared ("
            +std::to_string(stmt->m_ids.size())
            +") does not match the size of the tuple expression ("
            +std::to_string(tuple->value().size())
            +")";
        throw InterpreterException(msg);
    }

    int i = 0;
    for (auto &tok : stmt->m_ids) {
        if (tok->lexeme() != "_") {
            if (_const)
                tuple->value().at(i)->set_const();

            std::shared_ptr<earl::variable::Obj> var
                = std::make_shared<earl::variable::Obj>(stmt->m_ids.at(i).get(), tuple->value().at(i), stmt->m_attrs);
            ctx->variable_add(var);
        }
        ++i;
    }

    return std::make_shared<earl::value::Void>();
}

static std::shared_ptr<earl::value::Obj>
eval_stmt_let_wcustom_buffer_in_class(StmtLet *stmt,
                             std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> &buffer,
                             std::shared_ptr<Ctx> &ctx,
                             bool ref) {
    if (stmt->m_ids.size() > 1)
        return eval_stmt_let_wmultiple_vars_wcustom_buffer_in_class(stmt, buffer, ctx, ref);

    assert(ctx->type() == CtxType::Class);

    const std::string &id = stmt->m_ids.at(0)->lexeme();
    if (dynamic_cast<ClassCtx *>(ctx.get())->variable_exists_wo__m_class_constructor_tmp_args(id)) {
        std::string msg = "variable `"+id+"` is already declared";
        auto conflict = ctx->variable_get(id);
        Err::err_wconflict(stmt->m_ids.at(0).get(), conflict->gettok());
        throw InterpreterException(msg);
    }

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

    if (id == "_")
        return std::make_shared<earl::value::Void>();

    std::shared_ptr<earl::variable::Obj> var
        = std::make_shared<earl::variable::Obj>(stmt->m_ids.at(0).get(), value, stmt->m_attrs);
    ctx->variable_add(var);
    return std::make_shared<earl::value::Void>();
}

static std::shared_ptr<earl::value::Obj>
eval_class_instantiation(ExprFuncCall *expr,
                         const std::string &id,
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

            auto owner = dynamic_cast<ClassCtx*>(class_ctx.get())->get_owner();
            auto world_ctx = dynamic_cast<WorldCtx *>(dynamic_cast<ClassCtx *>(class_ctx.get())->get_world_owner().get());

            assert(world_ctx);
            assert(world_ctx->type() == CtxType::World);

            class_stmt = world_ctx->class_get(id);
        }
    }
    else
        class_stmt = dynamic_cast<WorldCtx *>(ctx.get())->class_get(id);

    auto class_ctx = std::make_shared<ClassCtx>(ctx);

    if (params.size() != class_stmt->m_constructor_args.size()) {
        std::string msg = "Class `" + id + "` expects " + std::to_string(class_stmt->m_constructor_args.size()) +
                          " arguments but " + std::to_string(params.size()) + " were supplied";
        Err::err_wexpr(expr);
        throw InterpreterException(msg);
    }

    auto klass = std::make_shared<earl::value::Class>(class_stmt, class_ctx);

    // Add the constructor arguments to a temporary pushed scope
    for (size_t i = 0; i < class_stmt->m_constructor_args.size(); ++i) {
        auto var = std::make_shared<earl::variable::Obj>(class_stmt->m_constructor_args[i].get(), params[i]);

        // MAKE SURE TO CLEAR AT THE END OF THIS FUNC!
        class_ctx->fill___m_class_constructor_tmp_args(var);
    }

    // Eval member variables
    for (auto &member : class_stmt->m_members)
        (void)eval_stmt_let_wcustom_buffer_in_class(member.get(),
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
        (void)eval_user_defined_function(nullptr, constructor_id, unused, klass->ctx());
    }

    // CLEARED!
    class_ctx->clear___m_class_constructor_tmp_args();

    return klass;
}

static std::vector<std::shared_ptr<earl::value::Obj>>
evaluate_function_parameters(ExprFuncCall *funccall, std::shared_ptr<Ctx> ctx, bool ref) {
    std::vector<std::shared_ptr<earl::value::Obj>> res = {};
    PackedERPreliminary perp(nullptr, /*this_=*/false, /*errtok=*/funccall->m_tok.get());
    for (size_t i = 0; i < funccall->m_params.size(); ++i) {
        ER er = Interpreter::eval_expr(funccall->m_params[i].get(), ctx, ref);
        res.push_back(unpack_ER(er, ctx, ref, /*perp=*/&perp));
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

            if (refs.size() != funccall->m_params.size()) {
                const std::string msg = "function `"+fun->id()+"` expects "+std::to_string(refs.size())+" arguments but got "+std::to_string(funccall->m_params.size());
                Err::err_wexpr(funccall);
                throw InterpreterException(msg);
            }

            // Evaluate the parameters based on the reference table
            for (size_t i = 0; i < funccall->m_params.size(); ++i) {
                ER er = Interpreter::eval_expr(funccall->m_params[i].get(), ctx, /*ref=*/refs.at(i));
                res.push_back(unpack_ER(er, ctx, refs[i]));
            }
        }

        else if constexpr (std::is_same_v<T, earl::value::Closure *>) {

            // Build reference table
            for (size_t i = 0; i < fun->params_len(); ++i)
                refs.push_back(static_cast<bool>(fun->param_at_is_ref(i)));

            if (refs.size() != funccall->m_params.size()) {
                const std::string msg = "closure `"+fun->tok()->lexeme()+"` expects "+std::to_string(refs.size())+" arguments but got "+std::to_string(funccall->m_params.size());
                Err::err_wexpr(funccall);
                throw InterpreterException(msg);
            }

            // Evaluate the parameters based on the reference table
            for (size_t i = 0; i < funccall->m_params.size(); ++i) {
                ER er = Interpreter::eval_expr(funccall->m_params[i].get(), ctx, /*ref=*/refs[i]);
                res.push_back(unpack_ER(er, ctx, refs[i]));
            }
        }

        else {
            std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
            throw InterpreterException(msg);
        }
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
    std::vector<bool> originally_was_const = {};
    std::vector<int> refs = {};
    std::variant<std::shared_ptr<earl::function::Obj>, earl::value::Closure *> v;

    if (ctx->function_exists(id)) {
        if ((flags & __SHOWFUNS) != 0)
            std::cout << "[EARL show-fun] " << id << '\n';

        auto func = ctx->function_get(id);

        v = func;
        if (from_outside && !func->is_pub()) {
            std::string msg = "function `" + id + "` does not contain the @pub attribute";
            Err::err_wexpr(funccall);
            throw InterpreterException(msg);
        }

        params = evaluate_function_parameters_wrefs(funccall, v, funccall_ctx);
        for (auto &p : params) {
            if (p->is_const())
                originally_was_const.push_back(true);
            else
                originally_was_const.push_back(false);
        }

        if (func->params_len() != params.size()) {
            const std::string msg = "function `"+func->id()+"` expects "+std::to_string(func->params_len())+" arguments but got "+std::to_string(params.size());
            Err::err_wexpr(funccall);
            throw InterpreterException(msg);
        }

        auto fctx = std::make_shared<FunctionCtx>(ctx, func->attrs());
        fctx->set_curfunc(id);
        func->load_parameters(params, fctx);

        // Recursion optimization
        if (ctx->type() == CtxType::Function) {
            if (fctx->get_curfuncid() == dynamic_cast<FunctionCtx *>(ctx.get())->get_curfuncid()) {
                fctx->setrec();
            }
        }

        std::shared_ptr<Ctx> mask = fctx;
        auto res = Interpreter::eval_stmt_block(func->block(), mask);

        for (size_t i = 0; i < originally_was_const.size(); ++i) {
            if (!originally_was_const[i])
                params[i]->unset_const();
        }

        return res;
    }
    else if (ctx->closure_exists(id)) {
        auto cl = ctx->variable_get(id);
        auto clctx = std::make_shared<ClosureCtx>(ctx);
        earl::value::Closure *clvalue = dynamic_cast<earl::value::Closure *>(cl->value().get());
        v = clvalue;
        params = evaluate_function_parameters_wrefs(funccall, v, funccall_ctx);
        if (clvalue->params_len() != params.size()) {
            const std::string msg = "closure `"+id+"` expects "+std::to_string(clvalue->params_len())+" arguments but got "+std::to_string(params.size());
            Err::err_wexpr(funccall);
            throw InterpreterException(msg);
        }
        clvalue->load_parameters(params, clctx);
        std::shared_ptr<Ctx> mask = clctx;
        return Interpreter::eval_stmt_block(clvalue->block(), mask);
    }

    Err::err_wexpr(funccall);

    std::string msg = "function `" + id + "` has not been defined\n";
    auto avail = ctx->get_available_function_names();
    msg += "did you mean: " + identifier_not_declared(id, avail) + "?";

    throw InterpreterException(msg);
    return nullptr; // unreachable
}

static std::shared_ptr<earl::value::Obj>
eval_user_defined_function(ExprFuncCall *expr,
                           const std::string &id,
                           std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           bool from_outside) {
    if (ctx->function_exists(id)) {
        if ((flags & __SHOWFUNS) != 0)
            std::cout << "[EARL show-fun] " << id << '\n';

        auto func = ctx->function_get(id);
        if (func->params_len() != params.size()) {
            const std::string msg = "function `"+func->id()+"` expects "+std::to_string(func->params_len())+" arguments but got "+std::to_string(params.size());
            Err::err_wexpr(expr);
            throw InterpreterException(msg);
        }
        if (from_outside && !func->is_pub()) {
            std::string msg = "function `"+id+"` does not contain the @pub attribute";
            if (expr)
                Err::err_wexpr(expr);
            throw InterpreterException(msg);
        }
        auto fctx = std::make_shared<FunctionCtx>(ctx, func->attrs());
        func->load_parameters(params, fctx);
        fctx->set_curfunc(id);
        func->load_parameters(params, fctx);

        if (ctx->type() == CtxType::Function) {
            if (fctx->get_curfuncid() == dynamic_cast<FunctionCtx *>(ctx.get())->get_curfuncid()) {
                fctx->setrec();
            }
        }

        std::shared_ptr<Ctx> mask = fctx;
        return Interpreter::eval_stmt_block(func->block(), mask);
    }
    else if (ctx->closure_exists(id)) {
        auto cl = ctx->variable_get(id);
        auto clctx = std::make_shared<ClosureCtx>(ctx);
        auto clvalue = dynamic_cast<earl::value::Closure *>(cl->value().get());
        if (clvalue->params_len() != params.size()) {
            const std::string msg = "closure `"+id+"` expects "+std::to_string(clvalue->params_len())+" arguments but got "+std::to_string(params.size());
            Err::err_wexpr(expr);
            throw InterpreterException(msg);
        }
        clvalue->load_parameters(params, clctx);
        std::shared_ptr<Ctx> mask = clctx;
        return Interpreter::eval_stmt_block(clvalue->block(), mask);
    }

    Err::err_wexpr(expr);

    std::string msg = "function `" + id + "` has not been defined\n";
    auto avail = ctx->get_available_function_names();
    msg += "did you mean: " + identifier_not_declared(id, avail) + "?";

    throw InterpreterException(msg);
    return nullptr; // unreachable
}

static std::shared_ptr<earl::value::Obj>
unpack_ER(ER &er, std::shared_ptr<Ctx> &ctx, bool ref, PackedERPreliminary *perp) {
    // CLASSES
    if (er.is_class_instant()) {
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(er.extra), ctx, ref);
        auto class_instantiation = eval_class_instantiation(static_cast<ExprFuncCall *>(er.extra), er.id, params, ctx, ref);
        return class_instantiation;
    }

    // FUNCTIONS/MEMBERS/INTRINSICS
    if (er.is_function_ident()) {
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(er.extra), er.ctx, ref);
        if (er.is_intrinsic()) {
            Expr *expr = nullptr;
            if (er.extra)
                expr = static_cast<Expr *>(er.extra);
            auto call = Intrinsics::call(er.id, params, ctx, expr);
            return call;
        }

        if (er.is_member_intrinsic() && (perp && perp->lhs_getter_accessor)) {
            if (Intrinsics::is_member_intrinsic(er.id, static_cast<int>(perp->lhs_getter_accessor->type()))) {
                Expr *expr = nullptr;
                if (er.extra) expr = static_cast<Expr *>(er.extra);
                return Intrinsics::call_member(er.id,
                        perp->lhs_getter_accessor->type(),
                        perp->lhs_getter_accessor,
                        params,
                        ctx,
                        expr);
            }
        }

        if (ctx->type() == CtxType::Class) {
            std::shared_ptr<earl::function::Obj> func = nullptr;
            if (ctx->function_exists(er.id))
                func = ctx->function_get(er.id);
            if ((!perp || !perp->this_) && (func && er.ctx != ctx && !func->is_pub())) {
                std::string msg = "member variable `" + func->id() + "` is missing the @pub attribute";
                if (er.extra) Err::err_wexpr(static_cast<Expr *>(er.extra));
                throw InterpreterException(msg);
            }

            auto call = eval_user_defined_function(static_cast<ExprFuncCall *>(er.extra),er.id, params, ctx);
            if (call->type() == earl::value::Type::Return)
                call = std::make_shared<earl::value::Void>();
            return call;
        }

        // Method does not exist
        if (perp && perp->lhs_getter_accessor) {
            if (er.extra) Err::err_wexpr(static_cast<Expr *>(er.extra));
            std::string msg = "method `" + er.id + "` is not a part of the given type `"+earl::value::type_to_str(perp->lhs_getter_accessor->type())+"`\n";
            // auto avail = ctx->get_available_function_names();
            msg += "did you mean: " + method_not_declared(er.id, perp->lhs_getter_accessor) + "?";
            throw InterpreterException(msg);
        }

        // We need to have this function to gen the parameters so we
        // know which ones need to be taken as a reference. NOTE: The
        // routine(s) above this may need this change as well.
        auto call = eval_user_defined_function_wo_params(er.id, static_cast<ExprFuncCall *>(er.extra), er.ctx, ctx);
        if (call->type() == earl::value::Type::Return)
            call = std::make_shared<earl::value::Void>();
        return call;
    }

    // LITERAL
    else if (er.is_literal()) {
        return er.value;
    }

    // IDENTIFIER
    else if (er.is_ident()) {
        // Check if it is an entry of an enum
        if (perp && perp->lhs_getter_accessor && perp->lhs_getter_accessor->type() == earl::value::Type::Enum) {
            auto lhs = dynamic_cast<earl::value::Enum *>(perp->lhs_getter_accessor.get());
            if (lhs->has_entry(er.id))
                return lhs->get_entry(er.id)->value()->copy();
        }
        if (ctx->variable_exists(er.id)) {
            auto var = ctx->variable_get(er.id);
            if ((!perp || !perp->this_) && (er.ctx != ctx && !var->is_pub())) {
                std::string msg = "member variable `"+var->id()+"` is missing the @pub attribute";
                if (er.extra) Err::err_wexpr(static_cast<Expr *>(er.extra));
                throw InterpreterException(msg);
            }
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

        // Check if it is a type as a value
        if (earl::value::is_typekw(er.id))
            return std::make_shared<earl::value::TypeKW>(earl::value::get_typekw_proper(er.id));

        if (earl::value::is_builtin_ident(er.id))
            return earl::value::get_builtin_ident(er.id, ctx);

        Err::err_wexpr(static_cast<Expr *>(er.extra));

        std::string msg = "variable `" + er.id + "` has not been defined\n";
        auto avail = ctx->get_available_variable_names();

        if (avail.size() != 0)
            msg += "did you mean: " + identifier_not_declared(er.id, avail, /*include_intrinsics=*/false) + "?";

        throw InterpreterException(msg);
    }

    // UNIT
    else if (er.is_wildcard())
        return std::make_shared<earl::value::Void>();
    else
        assert(false && "unreachable");
    return nullptr; // unreachable
}

static ER
eval_expr_term_ident(ExprIdent *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    (void)ref;
    const std::string &id = expr->m_tok->lexeme();
    if (id == "_")
        return ER(nullptr, ERT::Wildcard, /*id=*/id, /*extra=*/nullptr, /*ctx=*/ctx);
    return ER(nullptr, ERT::Ident, /*id=*/id, /*extra=*/expr, /*ctx=*/ctx);
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
        else {
            std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
            throw InterpreterException(msg);
        }
    }, expr->m_right);

    if (right_er.is_class_instant()) {
        assert(right_er.ctx->type() == CtxType::World);
        WorldCtx *world = dynamic_cast<WorldCtx *>(right_er.ctx.get());
        if (world->class_is_defined(right_er.id)) {
            if ((world->class_get(right_er.id)->m_attrs & static_cast<uint32_t>(Attr::Pub)) == 0) {
                Err::err_wexpr(expr);
                std::string msg = "class `"+right_er.id+"` in module `"+left_id+"` does not contain the @pub attribute";
                throw InterpreterException(msg);
            }
        }
        auto params = evaluate_function_parameters(static_cast<ExprFuncCall *>(right_er.extra), ctx, ref);
        auto class_instantiation = eval_class_instantiation(static_cast<ExprFuncCall *>(right_er.extra), right_er.id, params, right_er.ctx, ref);
        return ER(class_instantiation, ERT::Literal, /*id=*/"", /*extra=*/nullptr, /*ctx=*/ctx);
    }
    if (right_er.is_function_ident()) {
        // Check if the function has @pub attribute
        if (right_er.ctx->function_exists(right_er.id)) {
            if (!right_er.ctx->function_get(right_er.id)->is_pub()) {
                std::string msg = "function `"+right_er.id+"` in module `"+left_id+"` does not contain the @pub attribute";
                Err::err_wexpr(left_ident);
                throw InterpreterException(msg);
            }
        }
        // Check if the closure has @pub attribute
        else if (right_er.ctx->closure_exists(right_er.id)) {
            if (!right_er.ctx->function_get(right_er.id)->is_pub()) {
                std::string msg = "variable `"+right_er.id+"` in module `"+left_id+"` does not contain the @pub attribute";
                Err::err_wexpr(left_ident);
                throw InterpreterException(msg);
            }
        }
        auto func = eval_user_defined_function_wo_params(right_er.id, static_cast<ExprFuncCall *>(right_er.extra), ctx, right_er.ctx);
        return ER(func, ERT::Literal);
    }
    else if (right_er.is_ident()) {
        auto value = unpack_ER(right_er, other_ctx, ref);
        // Check if it is a variable
        if (right_er.ctx->variable_exists(right_er.id)) {
            if (!right_er.ctx->variable_get(right_er.id)->is_pub()) {
                std::string msg = "variable `"+right_er.id+"` in module `"+left_id+"` does not contain the @pub attribute";
                Err::err_wexpr(left_ident);
                throw InterpreterException(msg);
            }
            return ER(value, ERT::Literal);
        }
        // It must be an enum
        assert(value->type() == earl::value::Type::Enum);
        if (!dynamic_cast<earl::value::Enum *>(value.get())->is_pub()) {
            std::string msg = "enumeration `"+right_er.id+"` in module `"+left_id+"` does not contain the @pub attribute";
            Err::err_wexpr(left_ident);
            throw InterpreterException(msg);
        }
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
        else {
            std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
            throw InterpreterException(msg);
        }
    }, expr->m_right);

    if (left_er.id == "this") {
        if (ctx->type() != CtxType::Function && ctx->type() != CtxType::Closure) {
            std::string msg = "Must be in a function in a class context to use the `this` keyword";
            Err::err_wexpr(expr);
            throw InterpreterException(msg);
        }

        std::shared_ptr<earl::value::Obj> value = nullptr;

        if (ctx->type() == CtxType::Closure) {
            auto closure_ctx = dynamic_cast<ClosureCtx *>(ctx.get());
            if (!closure_ctx->in_class()) {
                std::string msg = "Must be in a class context when using the `this` keyword";
                Err::err_wexpr(expr);
                throw InterpreterException(msg);
            }
            PackedERPreliminary perp(nullptr, true);
            value = unpack_ER(right_er, closure_ctx->get_outer_class_owner_ctx(), /*ref=*/true, /*perp=*/&perp);
        }
        else if (ctx->type() == CtxType::Class) {
            PackedERPreliminary perp(nullptr, true);
            value = unpack_ER(right_er, ctx, /*ref=*/true, /*perp=*/&perp);
        }
        else if (ctx->type() == CtxType::Function) {
            auto fctx = dynamic_cast<FunctionCtx *>(ctx.get());

            if (!fctx->in_class()) {
                std::string msg = "Must be in a class context when using the `this` keyword";
                Err::err_wexpr(expr);
                throw InterpreterException(msg);
            }

            PackedERPreliminary perp(nullptr, true);
            value = unpack_ER(right_er, fctx->get_outer_class_owner_ctx(), /*ref=*/true, /*perp=*/&perp);
        }
        else {
            std::string msg = "Must be in a function in a class context to use the `this` keyword";
            Err::err_wexpr(expr);
            throw InterpreterException(msg);
        }
        return ER(value, ERT::Literal);
    }
    else {
        auto left_value = unpack_ER(left_er, ctx, true);
        PackedERPreliminary perp(left_value, /*this=*/false, /*errtok=*/expr->m_tok.get());
        std::shared_ptr<earl::value::Obj> value = nullptr;

        if (left_value->type() == earl::value::Type::Class) {
            // Class method/member. The right side (right_er) contains the actual call/identifier to be evaluated,
            // and we need the left (left_value)'s context with the preliminary value of (perp).
            // auto cctx = dynamic_cast<earl::value::Class *>(left_value.get())->ctx();
            // dynamic_cast<ClassCtx *>(cctx.get())->function_debug_dump();
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
    std::shared_ptr<earl::value::Char> value = nullptr;
    if (expr->m_tok->lexeme() == "\\n")
        value = std::make_shared<earl::value::Char>('\n');
    else if (expr->m_tok->lexeme() == "\\t")
        value = std::make_shared<earl::value::Char>('\t');
    else if (expr->m_tok->lexeme() == "\\r")
        value = std::make_shared<earl::value::Char>('\r');
    else if (expr->m_tok->lexeme() == "\\0")
        value = std::make_shared<earl::value::Char>('\0');
    else if (expr->m_tok->lexeme() == "\\\\")
        value = std::make_shared<earl::value::Char>('\\');
    else
        value = std::make_shared<earl::value::Char>(expr->m_tok->lexeme()[0]);
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

    auto left_value = unpack_ER(left_er, ctx, true);
    auto idx_value = unpack_ER(idx_er, ctx, true);

    if (left_value->type() == earl::value::Type::List) {
        auto list = dynamic_cast<earl::value::List *>(left_value.get());
        return ER(list->nth(idx_value, expr), static_cast<ERT>(ERT::Literal|ERT::ListAccess));
    }
    else if (left_value->type() == earl::value::Type::Str) {
        auto str = dynamic_cast<earl::value::Str *>(left_value.get());
        return ER(str->nth(idx_value, expr), static_cast<ERT>(ERT::Literal|ERT::ListAccess));
    }
    else if (left_value->type() == earl::value::Type::Tuple) {
        auto tuple = dynamic_cast<earl::value::Tuple *>(left_value.get());
        return ER(tuple->nth(idx_value, expr), static_cast<ERT>(ERT::Literal|ERT::TupleAccess));
    }
    else if (left_value->type() == earl::value::Type::DictInt) {
        auto dict = dynamic_cast<earl::value::Dict<int> *>(left_value.get());
        return ER(dict->nth(idx_value, expr), static_cast<ERT>(ERT::Literal|ERT::ListAccess));
    }
    else if (left_value->type() == earl::value::Type::DictStr) {
        auto dict = dynamic_cast<earl::value::Dict<std::string> *>(left_value.get());
        return ER(dict->nth(idx_value, expr), static_cast<ERT>(ERT::Literal|ERT::ListAccess));
    }
    else if (left_value->type() == earl::value::Type::DictChar) {
        auto dict = dynamic_cast<earl::value::Dict<char> *>(left_value.get());
        return ER(dict->nth(idx_value, expr), static_cast<ERT>(ERT::Literal|ERT::ListAccess));
    }
    else if (left_value->type() == earl::value::Type::DictFloat) {
        auto dict = dynamic_cast<earl::value::Dict<double> *>(left_value.get());
        return ER(dict->nth(idx_value, expr), static_cast<ERT>(ERT::Literal|ERT::ListAccess));
    }
    else {
        std::string msg = "cannot use `[]` on non-list, non-tuple, non-dict, or non-str type";
        Err::err_wexpr(expr);
        throw InterpreterException(msg);
    }
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
    for (auto &entry : expr->m_args) {
        if (entry.first->lexeme() != "_")
            args.push_back(std::make_pair(entry.first.get(), entry.second));
    }
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

    if (lvalue->type() != rvalue->type()) {
        std::string msg = "type mismatch for generating a range";
        Err::err_wexpr(expr->m_start.get());
        Err::err_wexpr(expr->m_end.get());
        throw InterpreterException(msg);
    }

    std::vector<std::shared_ptr<earl::value::Obj>> values = {};

    switch (lvalue->type()) {
    case earl::value::Type::Int: {
        int start = dynamic_cast<earl::value::Int *>(lvalue.get())->value();
        int end = dynamic_cast<earl::value::Int *>(rvalue.get())->value();
        if (expr->m_inclusive) {
            while (start <= end)
                values.push_back(std::make_shared<earl::value::Int>(start++));
        }
        else {
            while (start < end)
                values.push_back(std::make_shared<earl::value::Int>(start++));
        }
        return ER(std::make_shared<earl::value::List>(values), ERT::Literal);
    } break;
    case earl::value::Type::Char: {
        char start = dynamic_cast<earl::value::Char *>(lvalue.get())->value();
        char end = dynamic_cast<earl::value::Char *>(rvalue.get())->value();
        if (expr->m_inclusive) {
            while (start <= end)
                values.push_back(std::make_shared<earl::value::Char>(start++));
        }
        else {
            while (start < end)
                values.push_back(std::make_shared<earl::value::Char>(start++));
        }
        return ER(std::make_shared<earl::value::List>(values), ERT::Literal);
    }
    default: {
        std::string msg = "invalid type "+earl::value::type_to_str(lvalue->type())+"` for type range";
        Err::err_wexpr(expr->m_start.get());
        throw InterpreterException(msg);
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

static ER
eval_expr_term_slice(ExprSlice *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    std::shared_ptr<earl::value::Obj> s = nullptr, e = nullptr;

    if (expr->m_start.has_value()) {
        ER er = Interpreter::eval_expr(expr->m_start.value().get(), ctx, ref);
        s = unpack_ER(er, ctx, ref);
    }
    if (expr->m_end.has_value()) {
        ER er = Interpreter::eval_expr(expr->m_end.value().get(), ctx, ref);
        e = unpack_ER(er, ctx, ref);
    }

    if (!s)
        s = std::make_shared<earl::value::Void>();
    if (!e)
        e = std::make_shared<earl::value::Void>();

    if (s->type() != earl::value::Type::Void && s->type() != earl::value::Type::Int) {
        if (expr->m_start.has_value())
            Err::err_wexpr(expr->m_start.value().get());
        goto bad_type;
    }
    if (e->type() != earl::value::Type::Void && e->type() != earl::value::Type::Int) {
        if (expr->m_end.has_value())
            Err::err_wexpr(expr->m_end.value().get());
        goto bad_type;
    }

    return ER(std::make_shared<earl::value::Slice>(s, e), ERT::Literal);

bad_type:
    std::string msg = "array slices must be indexed with either type int or type unit";
    throw InterpreterException(msg);
    return ER(nullptr, ERT::None); // unreachable
}

static ER
eval_expr_term_dict(ExprDict *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    if (expr->m_values.size() == 0) {
        const std::string msg = "Cannot create a dictionary of size 0. Use the `Dict(TypeKW)` to get an empty dictionary.";
        Err::err_wexpr(expr);
        throw InterpreterException(msg);
    }

    // Get the values of the first insertion.
    ER first_key_er = Interpreter::eval_expr(expr->m_values.at(0).first.get(), ctx, false);
    ER first_value_er = Interpreter::eval_expr(expr->m_values.at(0).second.get(), ctx, false);
    auto first_key = unpack_ER(first_key_er, ctx, false);
    auto first_value = unpack_ER(first_value_er, ctx, false);

    // All keys MUST match the initial type.
    earl::value::Type ty = first_key->type();

    switch (ty) {
    case earl::value::Type::Int: {
        auto dict = std::make_shared<earl::value::Dict<int>>(ty);
        int __first_key = dynamic_cast<earl::value::Int *>(first_key.get())->value();
        dict->insert(__first_key, first_value);

        for (size_t i = 1; i < expr->m_values.size(); ++i) {
            ER key_er = Interpreter::eval_expr(expr->m_values.at(i).first.get(), ctx, false);
            ER value_er = Interpreter::eval_expr(expr->m_values.at(i).second.get(), ctx, false);
            auto key = unpack_ER(key_er, ctx, false);
            auto value = unpack_ER(value_er, ctx, false);

            if (key->type() != ty) {
                const std::string msg = "all keys must be the same type in dictionaries";
                Err::err_wexpr(expr->m_values.at(i).first.get());
                throw InterpreterException(msg);
            }

            int __key = dynamic_cast<earl::value::Int *>(key.get())->value();
            dict->insert(__key, value);
        }

        return ER(dict, ERT::Literal);
    } break;
    case earl::value::Type::Str: {
        auto dict = std::make_shared<earl::value::Dict<std::string>>(ty);
        std::string __first_key = dynamic_cast<earl::value::Str *>(first_key.get())->value();
        dict->insert(__first_key, first_value);

        for (size_t i = 1; i < expr->m_values.size(); ++i) {
            ER key_er = Interpreter::eval_expr(expr->m_values.at(i).first.get(), ctx, false);
            ER value_er = Interpreter::eval_expr(expr->m_values.at(i).second.get(), ctx, false);
            auto key = unpack_ER(key_er, ctx, false);
            auto value = unpack_ER(value_er, ctx, false);

            if (key->type() != ty) {
                const std::string msg = "all keys must be the same type in dictionaries";
                Err::err_wexpr(expr->m_values.at(i).first.get());
                throw InterpreterException(msg);
            }

            std::string __key = dynamic_cast<earl::value::Str *>(key.get())->value();
            dict->insert(__key, value);
        }

        return ER(dict, ERT::Literal);
    } break;
    case earl::value::Type::Char: {
        auto dict = std::make_shared<earl::value::Dict<char>>(ty);
        char __first_key = dynamic_cast<earl::value::Char *>(first_key.get())->value();
        dict->insert(__first_key, first_value);

        for (size_t i = 1; i < expr->m_values.size(); ++i) {
            ER key_er = Interpreter::eval_expr(expr->m_values.at(i).first.get(), ctx, false);
            ER value_er = Interpreter::eval_expr(expr->m_values.at(i).second.get(), ctx, false);
            auto key = unpack_ER(key_er, ctx, false);
            auto value = unpack_ER(value_er, ctx, false);

            if (key->type() != ty) {
                const std::string msg = "all keys must be the same type in dictionaries";
                Err::err_wexpr(expr->m_values.at(i).first.get());
                throw InterpreterException(msg);
            }

            char __key = dynamic_cast<earl::value::Char *>(key.get())->value();
            dict->insert(__key, value);
        }

        return ER(dict, ERT::Literal);
    } break;
    case earl::value::Type::Float: {
        auto dict = std::make_shared<earl::value::Dict<double>>(ty);
        double __first_key = dynamic_cast<earl::value::Float *>(first_key.get())->value();
        dict->insert(__first_key, first_value);

        for (size_t i = 1; i < expr->m_values.size(); ++i) {
            ER key_er = Interpreter::eval_expr(expr->m_values.at(i).first.get(), ctx, false);
            ER value_er = Interpreter::eval_expr(expr->m_values.at(i).second.get(), ctx, false);
            auto key = unpack_ER(key_er, ctx, false);
            auto value = unpack_ER(value_er, ctx, false);

            if (key->type() != ty) {
                const std::string msg = "all keys must be the same type in dictionaries";
                Err::err_wexpr(expr->m_values.at(i).first.get());
                throw InterpreterException(msg);
            }

            double __key = dynamic_cast<earl::value::Float *>(key.get())->value();
            dict->insert(__key, value);
        }

        return ER(dict, ERT::Literal);
    } break;
    default: {
        Err::err_wexpr(expr->m_values.at(0).first.get());
        const std::string msg = "type `"+earl::value::type_to_str(ty)+"` is not supported as a key in dictionaries";
        throw InterpreterException(msg);
    } break;
    }

    assert(false && "unreachable");
    return ER(nullptr, ERT::None); // unreachable
}

static ER
eval_expr_term_fstr(ExprFStr *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    const std::string &str = expr->m_tok->lexeme();
    auto result = std::make_shared<earl::value::Str>();

    auto until_closing = [](const std::string &s, size_t &it) -> std::string {
        std::string buf = "";
        while (it < s.size() && s[it] != '}') {
            buf.push_back(s.at(it));
            ++it;
        }
        return buf;
    };

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str.at(i);
        if (c == '{') {
            ++i;
            std::string id = until_closing(str, i);
            if (!ctx->variable_exists(id)) {
                Err::err_wexpr(expr);

                std::string msg = "variable `" + id + "` has not been defined\n";
                auto avail = ctx->get_available_variable_names();
                if (avail.size() != 0)
                    msg += "did you mean: " + identifier_not_declared(id, avail) + "?";

                throw InterpreterException(msg);
            }
            auto var = ctx->variable_get(id);
            std::string strified = var->value()->to_cxxstring();
            result->append(strified);
        }
        else
            result->append(c);
    }

    return ER(result, ERT::Literal);
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
    case ExprTermType::Slice:         return eval_expr_term_slice(dynamic_cast<ExprSlice *>(expr), ctx, ref);
    case ExprTermType::Dict:          return eval_expr_term_dict(dynamic_cast<ExprDict *>(expr), ctx, ref);
    case ExprTermType::FStr:          return eval_expr_term_fstr(dynamic_cast<ExprFStr *>(expr), ctx, ref);
    default: {
        std::string msg = "unknown term: `"+std::to_string((int)expr->get_term_type())+"`";
        throw InterpreterException(msg);
    }
    }
    assert(false && "unreachable");
    return ER(nullptr, ERT::None);
}

ER
eval_expr_bin(ExprBinary *expr, std::shared_ptr<Ctx> &ctx, bool ref) {
    ER lhs = Interpreter::eval_expr(expr->m_lhs.get(), ctx, ref);
    auto lhs_value = unpack_ER(lhs, ctx, true);

    // Short-circuit evaluation for logical AND (&&)
    if (expr->m_op->type() == TokenType::Double_Ampersand) {
        // If lhs is false, return lhs (no need to evaluate rhs)
        if (!lhs_value->boolean())
            return lhs;
        ER rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx, ref);
        return ER(unpack_ER(rhs, ctx, ref), ERT::Literal);
    }

    // Short-circuit evaluation for logical OR (||)
    if (expr->m_op->type() == TokenType::Double_Pipe) {
        // If lhs is true, return lhs (no need to evaluate rhs)
        if (lhs_value->boolean())
            return lhs;
        ER rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx, ref);
        return ER(unpack_ER(rhs, ctx, ref), ERT::Literal);
    }

    ER rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx, ref);
    auto rhs_value = unpack_ER(rhs, ctx, ref);
    // auto result = lhs_value->binop(expr->m_op.get(), rhs_value);
    std::shared_ptr<earl::value::Obj> result = nullptr;
    switch (expr->m_op.get()->type()) {
    case TokenType::Plus: {
        result = lhs_value->add(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Minus: {
        result = lhs_value->sub(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Asterisk: {
        result = lhs_value->multiply(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Forwardslash: {
        result = lhs_value->divide(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Percent: {
        result = lhs_value->modulo(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Double_Asterisk: {
        result = lhs_value->power(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Greaterthan:
    case TokenType::Lessthan:
    case TokenType::Greaterthan_Equals:
    case TokenType::Lessthan_Equals: {
        result = lhs_value->gtequality(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Double_Equals:
    case TokenType::Bang_Equals: {
        result = lhs_value->equality(expr->m_op.get(), rhs_value);
    } break;
    case TokenType::Backtick_Pipe:
    case TokenType::Backtick_Caret:
    case TokenType::Backtick_Ampersand: {
        result = lhs_value->bitwise(expr->m_op.get(), rhs_value);
    } break;
    default: {
        Err::err_wtok(expr->m_op.get());
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
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
        return eval_expr_term(dynamic_cast<ExprTerm *>(expr), ctx, ref);
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
eval_stmt_let_wmultiple_vars(StmtLet *stmt, std::shared_ptr<Ctx> &ctx) {
    if (ctx->type() == CtxType::Closure)
        // Special case for when we declare a variable in a recursive closure.
        for (auto &id : stmt->m_ids)
            dynamic_cast<ClosureCtx *>(ctx.get())->assert_variable_does_not_exist_for_recursive_cl(id->lexeme());
    else {
        int i = 0;
        for (auto &id : stmt->m_ids) {
            if (ctx->variable_exists(id->lexeme())) {
                std::string msg = "variable `"+id->lexeme()+"` is already declared";
                auto conflict = ctx->variable_get(id->lexeme());
                Err::err_wconflict(stmt->m_ids.at(i).get(), conflict->gettok());
                throw InterpreterException(msg);
                ++i;
            }
        }
    }

    bool ref = (stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;
    bool _const = (stmt->m_attrs & static_cast<uint32_t>(Attr::Const)) != 0;
    ER rhs = Interpreter::eval_expr(stmt->m_expr.get(), ctx, ref);

    std::shared_ptr<earl::value::Obj> value = nullptr;

    if (!rhs.is_class_instant()) {
        PackedERPreliminary perp(nullptr);
        value = unpack_ER(rhs, ctx, ref, /*perp=*/&perp);
    }
    else
        value = unpack_ER(rhs, ctx, ref);

    if (value->type() != earl::value::Type::Tuple) {
        Err::err_wexpr(stmt->m_expr.get());
        const std::string msg = "cannot declare multiple variables that do not equate to a tuple expression";
        throw InterpreterException(msg);
    }

    auto tuple = dynamic_cast<earl::value::Tuple *>(value.get());

    if (tuple->value().size() != stmt->m_ids.size()) {
        Err::err_wexpr(stmt->m_expr.get());
        const std::string msg =
            "the number of variables declared ("
            +std::to_string(stmt->m_ids.size())
            +") does not match the size of the tuple expression ("
            +std::to_string(tuple->value().size())
            +")";
        throw InterpreterException(msg);
    }

    int i = 0;
    for (auto &tok : stmt->m_ids) {
        if (tok->lexeme() != "_") {
            if (_const)
                tuple->value().at(i)->set_const();

            std::shared_ptr<earl::variable::Obj> var
                = std::make_shared<earl::variable::Obj>(stmt->m_ids.at(i).get(), tuple->value().at(i), stmt->m_attrs);
            ctx->variable_add(var);
        }
        ++i;
    }

    stmt->m_evald = true;
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx) {
    if (stmt->m_ids.size() > 1)
        return eval_stmt_let_wmultiple_vars(stmt, ctx);

    const std::string &id = stmt->m_ids.at(0)->lexeme();

    if (ctx->type() == CtxType::Closure)
        // Special case for when we declare a variable in a recursive closure.
        dynamic_cast<ClosureCtx *>(ctx.get())->assert_variable_does_not_exist_for_recursive_cl(id);
    else {
        if (ctx->variable_exists(id)) {
            std::string msg = "variable `"+id+"` is already declared";
            auto conflict = ctx->variable_get(id);
            Err::err_wconflict(stmt->m_ids.at(0).get(), conflict->gettok());
            throw InterpreterException(msg);
        }
    }

    bool ref = (stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;
    bool _const = (stmt->m_attrs & static_cast<uint32_t>(Attr::Const)) != 0;
    ER rhs = Interpreter::eval_expr(stmt->m_expr.get(), ctx, ref);

    std::shared_ptr<earl::value::Obj> value = nullptr;

    if (!rhs.is_class_instant()) {
        PackedERPreliminary perp(nullptr);
        if (rhs.is_list_access() && !ref)
            value = unpack_ER(rhs, ctx, ref, /*perp=*/&perp)->copy();
        else if (rhs.is_list_access())
            value = unpack_ER(rhs, ctx, ref, /*perp=*/&perp);
        else
            value = unpack_ER(rhs, ctx, ref, /*perp=*/&perp);
    }
    else
        value = unpack_ER(rhs, ctx, ref);

    if (value->type() == earl::value::Type::Return) {
    }

    if (id == "_")
        return std::make_shared<earl::value::Void>();

    if (_const || value->type() == earl::value::Type::Tuple)
        value->set_const();

    std::shared_ptr<earl::variable::Obj> var
        = std::make_shared<earl::variable::Obj>(stmt->m_ids.at(0).get(), value, stmt->m_attrs);
    ctx->variable_add(var);
    stmt->m_evald = true;
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_expr(StmtExpr *stmt, std::shared_ptr<Ctx> &ctx) {
    ER er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, false);
    stmt->m_evald = true;
    auto value = unpack_ER(er, ctx, false);
    if (value->type() == earl::value::Type::Return)
        std::cout << "HERE" << std::endl;
    if (value && value->type() != earl::value::Type::Void && ctx->type() != CtxType::World) {
        Err::err_wexpr(stmt->m_expr.get());
        Err::warn("Inplace expression will be evaluated and returned. Either explicitly `return` or assign the unused value to a unit binding: `let _ = <expr>;`");
    }
    return value;
}

std::shared_ptr<earl::value::Obj>
Interpreter::eval_stmt_block(StmtBlock *block, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> result = nullptr;
    ctx->push_scope();

    for (size_t i = 0; i < block->m_stmts.size(); ++i) {
        result = Interpreter::eval_stmt(block->m_stmts.at(i).get(), ctx);
        if (result && result->type() != earl::value::Type::Void)
            break;
        // if (block->m_stmts.at(i)->stmt_type() == StmtType::Return)
        //     break;
        if (block->m_stmts.at(i)->stmt_type() == StmtType::Return) {
            if (!result || result->type() == earl::value::Type::Void) {
                result = std::make_shared<earl::value::Return>();
            }
            break;
        }
    }

    ctx->pop_scope();
    block->m_evald = true;
    if (!result)
        result = std::make_shared<earl::value::Void>();
    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx) {
    const std::string &id = stmt->m_id->lexeme();
    if (ctx->function_exists(id)) {
        std::string msg = "function `"+id+"` has already been declared";
        auto conflict = ctx->function_get(id);
        Err::err_wconflict(stmt->m_id.get(), conflict->gettok());
        throw InterpreterException(msg);
    }

    std::vector<std::pair<Token *, uint32_t>> args;
    for (auto &entry : stmt->m_args)
        args.push_back(std::make_pair(entry.first.get(), entry.second));

    auto func = std::make_shared<earl::function::Obj>(stmt, args, stmt->m_id.get());
    ctx->function_add(func);
    stmt->m_evald = true;
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

    stmt->m_evald = true;
    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_return(StmtReturn *stmt, std::shared_ptr<Ctx> &ctx) {
    if (stmt->m_expr.has_value()) {
        ER er = Interpreter::eval_expr(stmt->m_expr.value().get(), ctx, false);
        stmt->m_evald = true;
        return unpack_ER(er, ctx, false);
    }
    stmt->m_evald = true;
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_break(StmtBreak *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    stmt->m_evald = true;
    return std::make_shared<earl::value::Break>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_mut(StmtMut *stmt, std::shared_ptr<Ctx> &ctx) {
    ER left_er = Interpreter::eval_expr(stmt->m_left.get(), ctx, true);
    ER right_er = Interpreter::eval_expr(stmt->m_right.get(), ctx, false);

    if (left_er.is_tuple_access()) {
        Err::err_wexpr(stmt->m_left.get());
        Err::err_wexpr(stmt->m_right.get());
        std::string msg = "cannot mutate tuple type as they are immutable";
        throw InterpreterException(msg);
    }

    auto l = unpack_ER(left_er, ctx, true);
    auto r = unpack_ER(right_er, ctx, false);

    switch (stmt->m_equals->type()) {
    case TokenType::Equals: {
        l->mutate(r, stmt);
    } break;
    case TokenType::Plus_Equals:
    case TokenType::Minus_Equals:
    case TokenType::Asterisk_Equals:
    case TokenType::Forwardslash_Equals:
    case TokenType::Percent_Equals:
    case TokenType::Backtick_Pipe_Equals:
    case TokenType::Backtick_Ampersand_Equals:
    case TokenType::Backtick_Caret_Equals: {
        l->spec_mutate(stmt->m_equals.get(), r, stmt);
    } break;
    default: {
        Err::err_wtok(stmt->m_equals.get());
        std::string msg = "invalid mutation operation `"+stmt->m_equals->lexeme()+"`";
        throw InterpreterException(msg);
    } break;
    }
    stmt->m_evald = true;
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

        if (result && result->type() == earl::value::Type::Continue)
            continue;

        if (result && result->type() != earl::value::Type::Void) {
            break;
        }

        expr_er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, /*ref=*/false);
        expr_result = unpack_ER(expr_er, ctx, /*ref=*/true);
        if (!expr_result->boolean())
            break;
    }

    if (result && (result->type() == earl::value::Type::Continue || result->type() == earl::value::Type::Break))
        result = std::make_shared<earl::value::Void>();

    stmt->m_evald = true;
    return result;
}

// This function is only used for eval_stmt_foreach() for creating enumerators from a tuple.
static void
destructure_enumerators(std::vector<std::shared_ptr<Token>> &named_ids,
                        std::vector<std::shared_ptr<earl::variable::Obj>> &vars,
                        const std::shared_ptr<earl::value::Obj> &values,
                        uint32_t attrs,
                        Expr *expr) {
    if (vars.size() == 1)
        vars[0] = std::make_shared<earl::variable::Obj>(named_ids[0].get(), values);
    else if (values->type() == earl::value::Type::Tuple) {
        auto tuple = dynamic_cast<earl::value::Tuple *>(values.get());

        if (vars.size() != tuple->value().size()) {
            Err::err_wexpr(expr);
            const std::string msg = "the number of enumerators ("
                +std::to_string(vars.size())
                +") does not match the expected number of expressions from the tuple ("
                +std::to_string(tuple->value().size())
                +")";
            throw InterpreterException(msg);
        }

        for (size_t i = 0; i < vars.size(); ++i) {
            auto value = tuple->value()[i];
            if ((attrs & static_cast<uint32_t>(Attr::Const)) != 0)
                value->set_const();
            vars[i] = std::make_shared<earl::variable::Obj>(named_ids[i].get(), tuple->value()[i], attrs);
        }
    }
    else {
        Err::err_wexpr(expr);
        const std::string msg = "unable to destructure a non-tuple type";
        throw InterpreterException(msg);
    }
}

// This function is only used for eval_stmt_foreach() for resetting the enumerators values.
static void
reset_enumerators(std::vector<std::shared_ptr<earl::variable::Obj>> &vars,
                  const std::shared_ptr<earl::value::Obj> &values,
                  Expr *expr) {
    if (vars.size() == 1) {
        vars[0]->reset(values);
    }
    else if (values->type() == earl::value::Type::Tuple) {
        auto tuple = dynamic_cast<earl::value::Tuple *>(values.get());

        if (vars.size() != tuple->value().size()) {
            Err::err_wexpr(expr);
            const std::string msg = "the number of enumerators ("
                +std::to_string(vars.size())
                +") does not match the expected number of expressions from the tuple ("
                +std::to_string(tuple->value().size())
                +")";
            throw InterpreterException(msg);
        }

        for (size_t i = 0; i < vars.size(); ++i)
            vars[i]->reset(tuple->value()[i]);
    }
    else {
        Err::err_wexpr(expr);
        const std::string msg = "unable to destructure a non-tuple type";
        throw InterpreterException(msg);
    }
}

std::shared_ptr<earl::value::Obj>
eval_stmt_foreach(StmtForeach *stmt, std::shared_ptr<Ctx> &ctx) {
    bool ref = (stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;

    std::shared_ptr<earl::value::Obj> result = nullptr;
    ER expr_er = Interpreter::eval_expr(stmt->m_expr.get(), ctx, ref);
    auto expr = unpack_ER(expr_er, ctx, ref);

    for (auto &enumer : stmt->m_enumerators) {
        const std::string &id = enumer->lexeme();
        if (ctx->variable_exists(id)) {
            std::string msg = "variable `"+id+"` is already declared";
            auto conflict = ctx->variable_get(id);
            Err::err_wconflict(enumer.get(), conflict->gettok());
            throw InterpreterException(msg);
        }
    }

    std::vector<std::shared_ptr<earl::variable::Obj>> enumerators(stmt->m_enumerators.size(), nullptr);
    auto wrapped_iterator = expr->iter_begin(), wrapped_iterator_end = expr->iter_end();

    // Will reduce the type of the current iterator and will
    // call handle_enumerators() on the enumerators of the foreach loop.
    auto iterator_reduce = [&](const auto &it, auto handle_enumerators) {
        std::visit([&](const auto &it){
            using T = std::decay_t<decltype(it)>;

            if constexpr (std::is_same_v<T, earl::value::ListIterator> ||
                          std::is_same_v<T, earl::value::StrIterator>) {
                handle_enumerators(*it);
            }
            else {
                static_assert(std::is_same_v<T, earl::value::DictIntIterator> ||
                              std::is_same_v<T, earl::value::DictCharIterator> ||
                              std::is_same_v<T, earl::value::DictFloatIterator> ||
                              std::is_same_v<T, earl::value::DictStrIterator>);
                std::vector<std::shared_ptr<earl::value::Obj>> elements = {};
                if constexpr (std::is_same_v<T, earl::value::DictIntIterator>)
                    elements.push_back(std::make_shared<earl::value::Int>(it->first));
                else if constexpr (std::is_same_v<T, earl::value::DictCharIterator>)
                    elements.push_back(std::make_shared<earl::value::Char>(it->first));
                else if constexpr (std::is_same_v<T, earl::value::DictFloatIterator>)
                    elements.push_back(std::make_shared<earl::value::Float>(it->first));
                else if constexpr (std::is_same_v<T, earl::value::DictStrIterator>)
                    elements.push_back(std::make_shared<earl::value::Str>(it->first));
                else {
                    Err::err_wexpr(stmt->m_expr.get());
                    const std::string msg = "unknown dictionary iterator type";
                    throw InterpreterException(msg);
                }
                elements.push_back(it->second);
                auto tuple = std::make_shared<earl::value::Tuple>(elements);
                handle_enumerators(tuple);
            }
        }, it);
    };

    // The container we are iterating over is empty, nothing to do.
    if (wrapped_iterator == wrapped_iterator_end)
        goto done;

    // Setup starting enumerators
    iterator_reduce(wrapped_iterator, [&](auto &tuple){
        destructure_enumerators(stmt->m_enumerators, enumerators, tuple, stmt->m_attrs, stmt->m_expr.get());
    });
    for (size_t i = 0; i < enumerators.size(); ++i)
        ctx->variable_add(enumerators[i]);

    // Main loop
    while (true) {

        // Have we enumerated over all elements?
        if (std::visit([&](const auto &it) {
            return it == std::get<std::decay_t<decltype(it)>>(wrapped_iterator_end);
        }, wrapped_iterator)) {
            break;
        }

        // Get new values into the enumerators.
        iterator_reduce(wrapped_iterator, [&](auto &tuple){reset_enumerators(enumerators, tuple, stmt->m_expr.get());});
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);

        if (result && result->type() == earl::value::Type::Break) {
            result = nullptr;
            break;
        }
        if (result && result->type() == earl::value::Type::Continue) {
            expr->iter_next(wrapped_iterator);
            continue;
        }
        if (result && result->type() != earl::value::Type::Void)
            break;

        expr->iter_next(wrapped_iterator);
    }

    for (size_t i = 0; i < enumerators.size(); ++i)
        ctx->variable_remove(enumerators[i]->id());

 done:

    if (result && (result->type() == earl::value::Type::Continue || result->type() == earl::value::Type::Break))
        result = std::make_shared<earl::value::Void>();

    stmt->m_evald = true;
    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_for(StmtFor *stmt, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> result = nullptr;

    ER start_er = Interpreter::eval_expr(stmt->m_start.get(), ctx, false);
    ER end_er = Interpreter::eval_expr(stmt->m_end.get(), ctx, false);

    auto start_expr = unpack_ER(start_er, ctx, false); // DO NOT MAKE THIS TRUE! BREAKS LOOPS ENTIRELY
    auto end_expr = unpack_ER(end_er, ctx, true); // POSSIBLE BREAK, WAS FALSE

    auto enumerator = std::make_shared<earl::variable::Obj>(stmt->m_enumerator.get(), start_expr);

    if (ctx->variable_exists(enumerator->id())) {
        std::string msg = "variable `"+stmt->m_enumerator->lexeme()+"` is already declared";
        auto conflict = ctx->variable_get(enumerator->id());
        Err::err_wconflict(stmt->m_enumerator.get(), conflict->gettok());
        throw InterpreterException(msg);
    }
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

        if (result && result->type() == earl::value::Type::Continue) {
            if (lt)
                start->mutate(std::make_shared<earl::value::Int>(start->value()+1), nullptr);
            else if (gt)
                start->mutate(std::make_shared<earl::value::Int>(start->value()-1), nullptr);
            continue;
        }

        if (result && result->type() != earl::value::Type::Void)
            break;

        if (lt)
            start->mutate(std::make_shared<earl::value::Int>(start->value()+1), nullptr);
        else if (gt)
            start->mutate(std::make_shared<earl::value::Int>(start->value()-1), nullptr);
    }

    ctx->variable_remove(enumerator->id());

    if (result && (result->type() == earl::value::Type::Continue || result->type() == earl::value::Type::Break))
        result = std::make_shared<earl::value::Void>();

    stmt->m_evald = true;
    return result;
}

std::shared_ptr<earl::value::Obj>
eval_stmt_class(StmtClass *stmt, std::shared_ptr<Ctx> &ctx) {
    dynamic_cast<WorldCtx *>(ctx.get())->define_class(stmt);
    stmt->m_evald = true;
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_mod(StmtMod *stmt, std::shared_ptr<Ctx> &ctx) {
    dynamic_cast<WorldCtx *>(ctx.get())->set_mod(stmt->m_id->lexeme());
    stmt->m_evald = true;
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
eval_stmt_import(StmtImport *stmt, std::shared_ptr<Ctx> &ctx) {
    if (ctx->type() != CtxType::World) {
        Err::err_wtok(stmt->m_fp.get());
        std::string msg = "`import` statements must be used in the @world context";
        throw InterpreterException(msg);
    }

    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types    = {};
    std::string comment               = COMMON_EARL_COMMENT;

    std::string src_code              = read_file(stmt->m_fp.get()->lexeme().c_str());
    std::unique_ptr<Lexer> lexer      = lex_file(src_code,
                                                 stmt->m_fp.get()->lexeme(),
                                                 keywords,
                                                 types,
                                                 comment);
    std::unique_ptr<Program> program  = Parser::parse_program(*lexer.get(), stmt->m_fp->lexeme());

    std::shared_ptr<Ctx> child_ctx = Interpreter::interpret(std::move(program), std::move(lexer));
    assert(child_ctx->type() == CtxType::World);
    if (stmt->__m_depth == COMMON_DEPTH_ALMOST)
        dynamic_cast<WorldCtx *>(child_ctx.get())->strip_funs_and_classes();
    dynamic_cast<WorldCtx *>(ctx.get())->add_import(std::move(child_ctx));
    stmt->m_evald = true;
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
        const std::string msg = "variable `"+ident->m_tok->lexeme()+"` in match statement is already declared";
        throw InterpreterException(msg);
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
                            stmt->m_evald = true;
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
                        stmt->m_evald = true;
                        return Interpreter::eval_stmt_block(branch->m_block.get(), ctx);
                    }
                }
            }
        }
    }

    stmt->m_evald = true;
    return nullptr;
}

static std::shared_ptr<earl::value::Obj>
eval_stmt_enum(StmtEnum *stmt, std::shared_ptr<Ctx> &ctx) {
    if (ctx->type() != CtxType::World) {
        std::string msg = "enum statements are only allowed in the @world scope";
        Err::err_wtok(stmt->m_id.get());
        throw InterpreterException(msg);
    }

    WorldCtx *wctx = dynamic_cast<WorldCtx *>(ctx.get());

    if (wctx->enum_exists(stmt->m_id->lexeme())) {
        Err::err_wtok(stmt->m_id.get());
        std::string msg = "enum `"+stmt->m_id->lexeme()+"` is already declared";
        throw InterpreterException(msg);
    }

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
            if (mixed_types) {
                std::string msg = "if using datatypes other than integers inside of an enum, all entries must be explicitly assigned";
                Err::err_wtok(p.first.get());
                Err::err_wexpr(p.second.get());
                throw InterpreterException(msg);
            }
            int actual = 0;
            if (last_value)
                actual = last_value->value()+1;
            auto value = std::make_shared<earl::value::Int>(actual);
            last_value = value.get();
            var = std::make_shared<earl::variable::Obj>(p.first.get(), std::shared_ptr<earl::value::Obj>(value));
        }
        elems.insert({p.first->lexeme(), std::move(var)});
    }

    if (mixed_types && found_unassigned) {
        std::string msg = "if using datatypes other than integers inside of an enum, all entries must be explicitly assigned";
        Err::err_wtok(stmt->m_id.get());
        throw InterpreterException(msg);
    }

    auto _enum = std::make_shared<earl::value::Enum>(stmt, std::move(elems), stmt->m_attrs);
    wctx->enum_add(std::move(_enum));
    stmt->m_evald = true;
    return std::make_shared<earl::value::Void>();
}

static std::shared_ptr<earl::value::Obj>
eval_stmt_continue(Stmt *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    stmt->m_evald = true;
    return std::make_shared<earl::value::Continue>();
}

static std::shared_ptr<earl::value::Obj>
eval_stmt_loop(StmtLoop *stmt, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> result = nullptr;

    while (1) {
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);

        if (result && result->type() == earl::value::Type::Break) {
            result = nullptr;
            break;
        }

        if (result && result->type() == earl::value::Type::Continue)
            continue;

        if (result && result->type() != earl::value::Type::Void)
            break;
    }

    if (result && (result->type() == earl::value::Type::Continue || result->type() == earl::value::Type::Break))
        result = std::make_shared<earl::value::Void>();

    stmt->m_evald = true;

    return result;
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
    case StmtType::Continue:  return eval_stmt_continue(dynamic_cast<StmtContinue *>(stmt), ctx);
    case StmtType::Loop:      return eval_stmt_loop(dynamic_cast<StmtLoop *>(stmt), ctx);
    default: assert(false && "unreachable");
    }
    std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
    throw InterpreterException(msg);
    return nullptr;
}

std::shared_ptr<Ctx>
Interpreter::interpret(std::unique_ptr<Program> program, std::unique_ptr<Lexer> lexer) {
    std::shared_ptr<Ctx> ctx = std::make_shared<WorldCtx>(std::move(lexer), std::move(program));
    WorldCtx *wctx = dynamic_cast<WorldCtx *>(ctx.get());

    // Collect all function definitions and class definitions first...
    // Also check to make sure the first statement is a module declaration.
    for (size_t i = 0; i < wctx->stmts_len(); ++i) {
        Stmt *stmt = wctx->stmt_at(i);
        if (i == 0 && stmt->stmt_type() != StmtType::Mod && ((flags & __REPL) == 0))
            WARN("A `module` statement is expected to be the first statement. "
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
            && stmt->stmt_type() != StmtType::Import) {
            (void)Interpreter::eval_stmt(stmt, ctx);
        }
    }

    return ctx;
}
