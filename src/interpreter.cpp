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

std::shared_ptr<earl::value::Obj> eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx);
std::shared_ptr<earl::value::Obj> eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx);

std::shared_ptr<earl::value::Obj> eval_user_defined_closure(std::shared_ptr<earl::variable::Obj> var, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)var;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("eval_user_defined_closure");
}

std::shared_ptr<earl::value::Obj> eval_user_defined_function(std::shared_ptr<earl::function::Obj> func, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &from_ctx) {
    std::shared_ptr<Ctx> new_ctx = from_ctx->new_instance(CtxType::Function);
    new_ctx->set_parent(from_ctx);
    func->load_parameters(params, from_ctx, new_ctx);
    return Interpreter::eval_stmt_block(func->block(), new_ctx);
}

std::shared_ptr<earl::value::Obj> eval_class_instantiation(const std::string &id, ExprFuncCall *funccall, std::shared_ptr<Ctx> &ctx) {
    StmtClass *class_stmt = ctx->class_stmt_get(id);

    if (funccall->m_params.size() != class_stmt->m_constructor_args.size())
        ERR_WARGS(Err::Type::Fatal, "The constructor for class `%s` expects %zu arguments but got %zu",
                  id.c_str(), class_stmt->m_constructor_args.size(), funccall->m_params.size());

    std::vector<std::shared_ptr<earl::value::Obj>> params = {};
    for (auto &param : funccall->m_params) {
        ER param_eval = Interpreter::eval_expr(param.get(), ctx);
        std::shared_ptr<earl::value::Obj> actual_value = nullptr;
        if (param_eval.is_ident()) {
            if (!ctx->var_exists(param_eval.id))
                ERR_WARGS(Err::Type::Undeclared, "variable `%s` is not defined", param_eval.id.c_str());
            actual_value = ctx->var_get(param_eval.id)->value();
        }
        else
            actual_value = param_eval.value;
        params.push_back(actual_value);
    }

    // Begin class construction
    auto klass = std::make_shared<earl::value::Class>(class_stmt, ctx);

    // Get all temporary constructor variables
    for (size_t i = 0; i < class_stmt->m_constructor_args.size(); ++i) {
        auto var = std::make_shared<earl::variable::Obj>(class_stmt->m_constructor_args[i].get(), params[i]);
        klass->ctx()->fill_pipe(var);
    }

    // Eval memeber variables
    for (auto &member : class_stmt->m_members)
        eval_stmt_let(member.get(), klass->ctx());

    // Eval methods
    for (size_t i = 0; i < class_stmt->m_methods.size(); ++i)
        eval_stmt_def(class_stmt->m_methods[i].get(), klass->ctx());

    // assert(false);
    klass->ctx()->clear_pipe();
    return klass;
}

std::shared_ptr<earl::value::Obj> eval_expr_list_literal(ExprListLit *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_list_literal");
}

std::shared_ptr<earl::value::Obj> eval_expr_get(ExprGet *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_get");
}

std::shared_ptr<earl::value::Obj> eval_expr_array_access(ExprArrayAccess *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_array_access");
}

std::shared_ptr<earl::value::Obj> eval_user_defined_function_from_identifier(ExprFuncCall *funccall, const std::string &id, std::shared_ptr<Ctx> &from_ctx) {
    auto func = from_ctx->func_get(id);

    std::shared_ptr<Ctx> new_ctx = from_ctx->new_instance(CtxType::Function);
    new_ctx->set_parent(from_ctx);

    std::vector<std::shared_ptr<earl::value::Obj>> params = {};
    for (auto &param : funccall->m_params) {
        ER param_eval = Interpreter::eval_expr(param.get(), from_ctx);
        std::shared_ptr<earl::value::Obj> actual_value = nullptr;
        if (param_eval.is_ident()) {
            if (!from_ctx->var_exists(param_eval.id))
                ERR_WARGS(Err::Type::Undeclared, "variable `%s` is not defined", param_eval.id.c_str());
            actual_value = from_ctx->var_get(param_eval.id)->value();
        }
        else
            actual_value = param_eval.value;
        params.push_back(actual_value);
    }

    func->load_parameters(params, from_ctx, new_ctx);
    return Interpreter::eval_stmt_block(func->block(), new_ctx);
}

ER eval_expr_term(ExprTerm *expr, std::shared_ptr<Ctx> &ctx) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
        auto ident = dynamic_cast<ExprIdent *>(expr);
        const std::string &id = ident->m_tok->lexeme();

        if (Intrinsics::is_intrinsic(id))
            return ER(nullptr, ERT::IntrinsicFunction, id);
        if (Intrinsics::is_member_intrinsic(id)) {
            return ER(nullptr, ERT::IntrinsicMemberFunction, id);
        }

        // NOTE: it is up to the caller to deal with the
        // identifier that is requested. This is why we are
        // returning nullptr with ERT of type Ident as well as the id.
        return ER(nullptr, ERT::Ident, id);
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
        auto value = std::make_shared<earl::value::Char>(dynamic_cast<ExprCharLit *>(expr)->m_tok->lexeme());
        return ER(value, ERT::Literal);
    } break;
    case ExprTermType::Func_Call: {
        auto funccall = dynamic_cast<ExprFuncCall *>(expr);
        ER left_er = Interpreter::eval_expr(funccall->m_left.get(), ctx);

        // The function is an intrinsic function call
        if (left_er.is_intrinsic())
            return ER(Intrinsics::call(left_er.id, funccall, ctx), ERT::Literal);

        if (left_er.is_member_intrinsic()) {
            // We need to not evaluate the function call as we do not know
            // what the left side of the `.` (get) operation is. This is
            // dealt with in eval_expr_term: case ExprTermType::Get.
            return ER(nullptr, ERT::IntrinsicMemberFunction, /*id=*/left_er.id, /*extra=*/dynamic_cast<void *>(funccall));
        }

        // The function is a user defined function
        else if (left_er.is_ident()) {
            if (ctx->class_stmt_exists(left_er.id))
                return ER(eval_class_instantiation(left_er.id, funccall, ctx), ERT::Literal, /*id=*/left_er.id);

            if (!ctx->func_exists(left_er.id))
                // NOTE: it is up to the caller to deal with the
                // identifier that is requested since it was not
                // found in the current context. This means that
                // it may be in a different context. This is why
                // we don't want to crash here.
                return ER(nullptr, ERT::FuncIdent, /*id=*/left_er.id, /*extra=*/(void*)funccall);

            return ER(eval_user_defined_function_from_identifier(funccall, left_er.id, ctx), ERT::Literal);
        }
        else
            UNIMPLEMENTED("left_er is other");
    } break;
    case ExprTermType::List_Literal: {
        assert(false);
    } break;
    case ExprTermType::Get: {
        auto get = dynamic_cast<ExprGet *>(expr);
        ER left_er = Interpreter::eval_expr(get->m_left.get(), ctx);
        ER right_er = Interpreter::eval_expr(get->m_right.get(), ctx);

        if (left_er.is_func_ident()) {
            left_er.value = eval_user_defined_function_from_identifier(static_cast<ExprFuncCall *>(left_er.extra), left_er.id, ctx);
            assert(false);
        }

        if (left_er.is_ident())
            // We need to assign the value from this current context's
            // list of variables
            left_er.value = ctx->var_get(left_er.id, /*crash_on_failure=*/true)->value();

        if (right_er.is_func_ident()) {
            assert(ctx->var_exists(left_er.id));
            auto var = ctx->var_get(left_er.id);
            assert(var->value()->type() == earl::value::Type::Class);

            auto klass = dynamic_cast<earl::value::Class *>(var->value().get());
            assert(klass->ctx()->func_exists(right_er.id));

            auto func = klass->ctx()->func_get(right_er.id);
            auto res = eval_user_defined_function_from_identifier(static_cast<ExprFuncCall *>(right_er.extra), right_er.id, klass->ctx());
            return ER(res, ERT::Literal);
        }

        if (right_er.is_ident()) {
            assert(false && "unimplemented");
        }

        if (right_er.is_member_intrinsic()) {
            assert(left_er.value);
            return ER(Intrinsics::call_member(right_er.id, left_er.value, static_cast<ExprFuncCall *>(right_er.extra), ctx), ERT::Literal);
        }

        assert(false);
    } break;
    case ExprTermType::Array_Access: {
        assert(false);
    } break;
    case ExprTermType::Bool: {
        auto boolean = dynamic_cast<ExprBool *>(expr);
        return ER(std::make_shared<earl::value::Bool>(boolean->m_value), ERT::Literal);
    } break;
    case ExprTermType::None: {
        assert(false);
    } break;
    case ExprTermType::Closure: {
        assert(false);
    } break;
    case ExprTermType::Tuple: {
        assert(false);
    } break;
    case ExprTermType::Mod_Access: {
        auto mod = dynamic_cast<ExprModAccess *>(expr);
        const std::string &id = mod->m_expr_ident->m_tok->lexeme();
        auto child = ctx->get_child_ctx(id);

        // `res` should be none, and should have the ID of the
        // requested identifier from the other module.
        auto function_from_module = Interpreter::eval_expr(mod->m_right.get(), ctx);
        if (function_from_module.is_none()) {
            assert(child->func_get(function_from_module.id));
            child->fill_buffer(ctx);
            auto res = eval_user_defined_function_from_identifier(static_cast<ExprFuncCall*>(function_from_module.extra), function_from_module.id, child);
            child->clear_buffer();
            return ER(res, ERT::Literal);
        }
        else if (function_from_module.is_func_ident()) {
            // Class instantiation or function from other module
            if (child->func_exists(function_from_module.id)) {
                auto res = eval_user_defined_function_from_identifier(static_cast<ExprFuncCall*>(function_from_module.extra), function_from_module.id, child);
                return ER(res, ERT::Literal);
            }
            else if (child->class_stmt_exists(function_from_module.id)) {
                child->fill_buffer(ctx);
                auto res = eval_class_instantiation(function_from_module.id, static_cast<ExprFuncCall*>(function_from_module.extra), child);
                child->clear_buffer();
                return ER(res, ERT::Literal);
            }
            else
                ERR_WARGS(Err::Type::Undeclared, "Function `%s` is not defined", function_from_module.id.c_str());
        }
        else {
            std::cout << (int)function_from_module.rt << std::endl;
            assert(false && "unimplemented");
        }
        return function_from_module;
    } break;
    default:
        ERR_WARGS(Err::Type::Fatal, "unknown term: `%d`", (int)expr->get_term_type());
        assert(false && "unreachable");
    }

    assert(false && "unreachable");
    return ER(nullptr, ERT::None);
}

ER eval_expr_bin(ExprBinary *expr, std::shared_ptr<Ctx> &ctx) {
    ER lhs = Interpreter::eval_expr(expr->m_lhs.get(), ctx);
    // auto lhs_value = lhs.value;

    if (lhs.is_ident()) {
        assert(ctx->var_exists(lhs.id));
        lhs.value = ctx->var_get(lhs.id)->value();
    }

    // Short-circuit evaluation for logical AND (&&)
    if (expr->m_op->type() == TokenType::Double_Ampersand) {
        // If lhs is false (or zero), return lhs (no need to evaluate rhs)
        if (!lhs.value->boolean())
            return lhs;
        auto rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx);
        return rhs;
    }

    auto rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx);

    if (rhs.is_ident()) {
        assert(ctx->var_exists(rhs.id));
        rhs.value = ctx->var_get(rhs.id)->value();
    }

    auto result = lhs.value->binop(expr->m_op.get(), rhs.value);

    return ER(result, ERT::Literal);
}

ER Interpreter::eval_expr(Expr *expr, std::shared_ptr<Ctx> &ctx) {
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

std::shared_ptr<earl::value::Obj> eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx) {
    const std::string &id = stmt->m_id->lexeme();

    if (ctx->var_exists(id, /*check_pipe =*/false)) {
        Err::err_wtok(stmt->m_id.get());
        ERR_WARGS(Err::Type::Redeclaration, "variable `%s` already exists", id.c_str());
    }

    auto val = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    std::shared_ptr<earl::variable::Obj> var = nullptr;

    if (val.is_none())
        ERR_WARGS(Err::Type::Undeclared, "identifier `%s` has not been declared", val.id.c_str());

    if (val.is_ident()) {
        if (!ctx->var_exists(val.id))
            ERR_WARGS(Err::Type::Undeclared, "variable `%s` does not exist", val.id.c_str());
        val.value = ctx->var_get(val.id)->value();
    }

    if (val.is_literal() || (val.is_ident() && ((stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0)))
        // If the result is a literal, no need to copy.
        // If the result is not a literal, but we have a `ref` attr, no need to copy.
        var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), val.value, stmt->m_attrs);
    else
        // Copy the value
        var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), val.value->copy(), stmt->m_attrs);

    ctx->var_add(var);

    return nullptr;
}

std::shared_ptr<earl::value::Obj> eval_stmt_expr(StmtExpr *stmt, std::shared_ptr<Ctx> &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx).value;
}

std::shared_ptr<earl::value::Obj> Interpreter::eval_stmt_block(StmtBlock *block, std::shared_ptr<Ctx> &ctx) {
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

std::shared_ptr<earl::value::Obj> eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx) {
    if (ctx->func_exists(stmt->m_id->lexeme())) {
        ERR_WARGS(Err::Type::Redeclared,
                  "function `%s` is already declared", stmt->m_id->lexeme().c_str());
    }

    std::vector<std::pair<Token *, uint32_t>> args;

    for (auto &entry : stmt->m_args)
        args.push_back(std::make_pair(entry.first.get(), entry.second));

    auto created_function = std::make_shared<earl::function::Obj>(stmt, args);
    ctx->func_add(created_function);

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj> eval_stmt_if(StmtIf *stmt, std::shared_ptr<Ctx> &ctx) {
    auto expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    auto expr_value = expr_result.value;
    std::shared_ptr<earl::value::Obj> result = nullptr;

    if (expr_value->boolean())
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);
    else if (stmt->m_else.has_value())
        result = Interpreter::eval_stmt_block(stmt->m_else.value().get(), ctx);

    return result;
}

std::shared_ptr<earl::value::Obj> eval_stmt_return(StmtReturn *stmt, std::shared_ptr<Ctx> &ctx) {
    ER value = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    if (value.is_ident()) {
        if (!ctx->var_exists(value.id)) {
            ERR_WARGS(Err::Type::Undeclared, "variable `%s` is is not defined", value.id.c_str());
        }
        return ctx->var_get(value.id, /*crash_on_failure=*/true)->value();
    }
    return value.value;
}

std::shared_ptr<earl::value::Obj> eval_stmt_break(StmtBreak *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_break");
}

std::shared_ptr<earl::value::Obj> eval_stmt_mut(StmtMut *stmt, std::shared_ptr<Ctx> &ctx) {
    auto left = Interpreter::eval_expr(stmt->m_left.get(), ctx);
    auto right = Interpreter::eval_expr(stmt->m_right.get(), ctx);

    std::shared_ptr<earl::value::Obj> l, r;

    if (left.is_ident()) {
        assert(ctx->var_exists(left.id));
        l = ctx->var_get(left.id)->value();
    }
    else
        l = left.value;

    if (right.is_ident()) {
        assert(ctx->var_exists(right.id));
        r = ctx->var_get(right.id)->value();
    }
    else
        r = right.value;

    l->mutate(r);

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj> eval_stmt_while(StmtWhile *stmt, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> expr_result = nullptr;
    std::shared_ptr<earl::value::Obj> result = nullptr;

    while ((expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx).value)->boolean()) {
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);
        if (result && result->type() == earl::value::Type::Break) {
            result = nullptr;
            break;
        }

        if (result && result->type() != earl::value::Type::Void)
            break;
    }

    return result;
}

std::shared_ptr<earl::value::Obj> eval_stmt_for(StmtFor *stmt, std::shared_ptr<Ctx> &ctx) {
    std::shared_ptr<earl::value::Obj> result     = nullptr;
    auto start_expr = Interpreter::eval_expr(stmt->m_start.get(), ctx);
    auto end_expr   = Interpreter::eval_expr(stmt->m_end.get(), ctx);

    auto enumerator = std::make_shared<earl::variable::Obj>(stmt->m_enumerator.get(), start_expr.value);

    assert(!ctx->var_exists(enumerator->id()));
    ctx->var_add(enumerator);

    earl::value::Int *start = dynamic_cast<earl::value::Int *>(start_expr.value.get());
    earl::value::Int *end = dynamic_cast<earl::value::Int *>(end_expr.value.get());

    assert(start);
    assert(end);

    while (start->value() < end->value()) {
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);

        if (result && result->type() == earl::value::Type::Break) {
            result = nullptr;
            break;
        }

        if (result && result->type() != earl::value::Type::Void)
            break;

        std::shared_ptr<earl::value::Int> incr = std::make_shared<earl::value::Int>(start->value() + 1);
        start->mutate(incr);
    }

    ctx->var_remove(enumerator->id());

    return result;
}

std::shared_ptr<earl::value::Obj> eval_stmt_class(StmtClass *stmt, std::shared_ptr<Ctx> &ctx) {
    ctx->class_stmt_make_available(stmt);
    return nullptr;
}

std::shared_ptr<earl::variable::Obj> handle_match_some_branch(ExprFuncCall *expr, std::shared_ptr<earl::value::Obj> inject_value, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)inject_value;
    (void)ctx;
    UNIMPLEMENTED("handle_match_some_branch");
}

std::shared_ptr<earl::value::Obj> eval_stmt_match(StmtMatch *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_match");
}

std::shared_ptr<earl::value::Obj> eval_stmt_mod(StmtMod *stmt, std::shared_ptr<Ctx> &ctx) {
    ctx->set_module(stmt->m_id->lexeme());
    return nullptr;
}

std::shared_ptr<earl::value::Obj> eval_stmt_import(StmtImport *stmt, std::shared_ptr<Ctx> &ctx) {
    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types    = {};
    std::string comment               = COMMON_EARL_COMMENT;

    std::unique_ptr<Lexer> lexer      = lex_file(stmt->m_fp.get()->lexeme().c_str(), keywords, types, comment);
    std::unique_ptr<Program> program  = Parser::parse_program(*lexer.get());

    auto child_ctx = Interpreter::interpret(std::move(program), std::move(lexer));
    child_ctx->set_parent(ctx);
    ctx->push_child_context(child_ctx);
    return std::shared_ptr<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj> Interpreter::eval_stmt(Stmt *stmt, std::shared_ptr<Ctx> &ctx) {
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
        return eval_stmt_import(dynamic_cast<StmtImport *>(stmt), ctx);
    } break;
    case StmtType::Mod: {
        return eval_stmt_mod(dynamic_cast<StmtMod *>(stmt), ctx);
    } break;
    case StmtType::Class: {
        return eval_stmt_class(dynamic_cast<StmtClass *>(stmt), ctx);
    } break;
    case StmtType::Match: {
        return eval_stmt_match(dynamic_cast<StmtMatch *>(stmt), ctx);
    } break;
    default:
        assert(false && "unreachable");
    }

    assert(false && "unreachable");
}

std::shared_ptr<Ctx> Interpreter::interpret(std::unique_ptr<Program> program, std::unique_ptr<Lexer> lexer) {
    std::shared_ptr<Ctx> ctx = std::make_shared<Ctx>(std::move(lexer), std::move(program), CtxType::World);

    for (size_t i = 0; i < ctx->stmts_len(); ++i) {
        (void)Interpreter::eval_stmt(ctx->get_stmt_at(i), ctx);
    }

    return ctx;
}
