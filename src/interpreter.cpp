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
#include <unordered_map>
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

std::shared_ptr<earl::value::Obj> eval_user_defined_closure(std::shared_ptr<earl::variable::Obj> var, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)var;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("eval_user_defined_closure");
}

std::shared_ptr<earl::value::Obj> eval_user_defined_function(std::shared_ptr<earl::function::Obj> func, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    func->load_parameters(params, ctx);
    return Interpreter::eval_stmt_block(func->block(), ctx);
}

std::shared_ptr<earl::value::Obj> eval_user_defined_class_method(std::shared_ptr<earl::function::Obj> method, std::vector<std::shared_ptr<earl::value::Obj>> &params, earl::Class::Obj *klass, std::shared_ptr<Ctx> &ctx) {
    (void)method;
    (void)params;
    (void)klass;
    (void)ctx;
    UNIMPLEMENTED("eval_user_defined_class_method");
}

std::shared_ptr<earl::value::Obj> get_class_member(std::string &id, earl::Class::Obj *klass, std::shared_ptr<Ctx> &ctx) {
    (void)id;
    (void)klass;
    (void)ctx;
    UNIMPLEMENTED("get_class_member");
}

std::shared_ptr<earl::value::Obj> eval_stmt_let(StmtLet *stmt, std::shared_ptr<Ctx> &ctx) {
    const std::string &id = stmt->m_id->lexeme();

    if (ctx->var_exists(id)) {
        Err::err_wtok(stmt->m_id.get());
        ERR_WARGS(Err::Type::Redeclaration, "variable `%s` already exists", id.c_str());
    }

    auto val = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    std::shared_ptr<earl::variable::Obj> var = nullptr;

    // If the result is a literal, no need to copy.
    // If the result is not a literal, but we have a `ref` attr, no need to copy.
    if (val.is_literal() || (val.is_ident() && ((stmt->m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0))) {
        var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), val.value, stmt->m_attrs);
    }
    // Copy the value
    else {
        var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), val.value->copy(), stmt->m_attrs);
    }

    ctx->var_add(var);

    return nullptr;
}

void load_class_members(StmtLet *stmt, earl::Class::Obj *klass, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)klass;
    (void)ctx;
    UNIMPLEMENTED("load_class_members");
}

std::shared_ptr<earl::value::Obj> eval_class_instantiation(ExprFuncCall *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_class_instantiation");
}

std::shared_ptr<earl::value::Obj> eval_expr_funccall(ExprFuncCall *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_funccall");
}

std::shared_ptr<earl::value::Obj> eval_expr_list_literal(ExprListLit *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_list_literal");
}

std::shared_ptr<earl::value::Obj> eval_expr_get2(ExprGet *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_get2");
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

static ER eval_function_call(ExprFuncCall *funccall, std::shared_ptr<Ctx> &ctx, bool search_in_prev_ctx) {
    auto left_result = Interpreter::eval_expr(funccall->m_left.get(), ctx);
    auto left = left_result.value;

    std::vector<std::shared_ptr<earl::value::Obj>> params;
    std::for_each(funccall->m_params.begin(), funccall->m_params.end(), [&](auto &e) {
        auto param_result = Interpreter::eval_expr(e.get(), ctx, search_in_prev_ctx);
        if (param_result.is_none())
            ERR_WARGS(Err::Type::Undeclared, "unkown identifier `%s`", param_result.id.c_str());
        params.push_back(param_result.value);
    });

    if (left) {
        UNIMPLEMENTED("eval_expr_term:ExprTermType::Func_Call: if left");
    }

    if (funccall->m_left->get_type() == ExprType::Term) {
        if (dynamic_cast<ExprTerm *>(funccall->m_left.get())->get_term_type() == ExprTermType::Ident) {
            auto term = dynamic_cast<ExprIdent *>(funccall->m_left.get());
            const std::string &id = term->m_tok->lexeme();
            if (Intrinsics::is_intrinsic(id)) {
                auto value = Intrinsics::call(id, funccall, params, ctx);
                return ER(value, ERT::Literal);
            }
            else if (Intrinsics::is_member_intrinsic(id)) {
                auto member_value = Intrinsics::call_member(id, left, params, ctx);
                return ER(member_value, ERT::Literal);
            }
            else {
                // The function is an identifier, but not intrinsic
                auto func = ctx->func_get(id, /*crash_on_failure =*/true);
                auto fctx = ctx->new_instance(CtxType::Function);
                fctx->set_parent(ctx);

                auto value = eval_user_defined_function(func, params, fctx);
                return ER(value, ERT::Literal);
            }
        }
        else {
            assert(false && "unimplemented");
        }
    }
    else {
        assert(false && "unimplemented");
    }

    assert(false && "unimplemented");
}

ER eval_expr_term(ExprTerm *expr, std::shared_ptr<Ctx> &ctx, bool search_in_prev_ctx = false) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
        auto ident = dynamic_cast<ExprIdent *>(expr);
        const std::string &id = ident->m_tok->lexeme();
        std::shared_ptr<earl::variable::Obj> var = nullptr;

        if (search_in_prev_ctx)
            var = ctx->get_parent()->var_get(id, /*crash_on_failure =*/false);
        else
            var = ctx->var_get(id, /*crash_on_failure =*/false);

        if (var)
            return ER(var->value(), ERT::Ident);

        if (Intrinsics::is_intrinsic(id))
            return ER(nullptr, ERT::IntrinsicFunction);

        // No variable found, move on...
        return ER(nullptr, ERT::None, id);
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
        return eval_function_call(dynamic_cast<ExprFuncCall *>(expr), ctx, search_in_prev_ctx);
    } break;
    case ExprTermType::List_Literal: {
        assert(false);
    } break;
    case ExprTermType::Get: {
        std::shared_ptr<earl::value::Obj> result = nullptr;

        auto get = dynamic_cast<ExprGet *>(expr);
        auto left_result_ = Interpreter::eval_expr(get->m_left.get(), ctx, search_in_prev_ctx);
        auto left_ = left_result_.value;
        auto right = dynamic_cast<ExprTerm *>(get->m_right.get());

        switch (right->get_term_type()) {
        case ExprTermType::Ident: {
            assert(false && "unimplemented");
        } break;
        case ExprTermType::Func_Call: {
            auto funccall = dynamic_cast<ExprFuncCall *>(right);
            auto left_result = Interpreter::eval_expr(funccall->m_left.get(), ctx);
            auto left = left_result.value;

            std::vector<std::shared_ptr<earl::value::Obj>> params;
            std::for_each(funccall->m_params.begin(), funccall->m_params.end(), [&](auto &e) {
                auto param_result = Interpreter::eval_expr(e.get(), ctx, search_in_prev_ctx);
                if (param_result.is_none())
                    ERR_WARGS(Err::Type::Undeclared, "unkown identifier `%s`", param_result.id.c_str());
                params.push_back(param_result.value);
            });

            if (left) {
                UNIMPLEMENTED("eval_expr_term:ExprTermType::Func_Call: if left");
            }


            if (funccall->m_left->get_type() == ExprType::Term) {
                if (dynamic_cast<ExprTerm *>(funccall->m_left.get())->get_term_type() == ExprTermType::Ident) {
                    auto term = dynamic_cast<ExprIdent *>(funccall->m_left.get());
                    const std::string &id = term->m_tok->lexeme();
                    if (Intrinsics::is_member_intrinsic(id)) {
                        auto member_value = Intrinsics::call_member(id, left_, params, ctx);
                        return ER(member_value, ERT::Literal);
                    }
                }
                else {
                    assert(false && "unimplemented");
                }
            }

            // return eval_function_call(dynamic_cast<ExprFuncCall *>(right), ctx, search_in_prev_ctx);
        } break;
        default: ERR(Err::Type::Fatal, "invalid `get` operation");
        }

        abort();
    } break;
    case ExprTermType::Array_Access: {
        assert(false);
    } break;
    case ExprTermType::Bool: {
        assert(false);
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
        auto ma = dynamic_cast<ExprModAccess *>(expr);
        const std::string &id = ma->m_expr_ident->m_tok->lexeme();
        auto child = ctx->get_child_ctx(id);
        return Interpreter::eval_expr(ma->m_right.get(), child, /*search_in_prev_ctx =*/true);
    } break;
    default:
        ERR_WARGS(Err::Type::Fatal, "unknown term: `%d`", (int)expr->get_term_type());
        assert(false && "unreachable");
    }

    assert(false && "unreachable");
    return ER(nullptr, ERT::None);
}

ER eval_expr_bin(ExprBinary *expr, std::shared_ptr<Ctx> &ctx) {
    auto lhs = Interpreter::eval_expr(expr->m_lhs.get(), ctx);

    // Short-circuit evaluation for logical AND (&&)
    if (expr->m_op->type() == TokenType::Double_Ampersand) {
        // If lhs is false (or zero), return lhs (no need to evaluate rhs)
        if (!lhs.value->boolean())
            return lhs;
        auto rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx);
        return rhs;
    }

    auto rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx);
    auto result = lhs.value->binop(expr->m_op.get(), rhs.value);

    return ER(result, ERT::Literal);
}

ER Interpreter::eval_expr(Expr *expr, std::shared_ptr<Ctx> &ctx, bool search_in_prev_ctx) {
    switch (expr->get_type()) {
    case ExprType::Term: {
        auto result = eval_expr_term(dynamic_cast<ExprTerm *>(expr), ctx, search_in_prev_ctx);
        return result;
    } break;
    case ExprType::Binary: {
        return eval_expr_bin(dynamic_cast<ExprBinary *>(expr), ctx);
    } break;
    default:
        assert(false && "unreachable");
    }
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

    for (auto &entry : stmt->m_args) {
        args.push_back(std::make_pair(entry.first.get(), entry.second));
    }

    auto created_function = std::make_shared<earl::function::Obj>(stmt, args);
    ctx->func_add(std::move(created_function));

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj> eval_stmt_if(StmtIf *stmt, std::shared_ptr<Ctx> &ctx) {
    auto expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    auto expr_value = expr_result.value;
    std::shared_ptr<earl::value::Obj> result = nullptr;

    if (expr_value->boolean()) {
        result = Interpreter::eval_stmt_block(stmt->m_block.get(), ctx);
    }
    else if (stmt->m_else.has_value()) {
        result = Interpreter::eval_stmt_block(stmt->m_else.value().get(), ctx);
    }

    return result;
}

std::shared_ptr<earl::value::Obj> eval_stmt_return(StmtReturn *stmt, std::shared_ptr<Ctx> &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx).value;
}

std::shared_ptr<earl::value::Obj> eval_stmt_break(StmtBreak *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_break");
}

std::shared_ptr<earl::value::Obj> eval_stmt_mut(StmtMut *stmt, std::shared_ptr<Ctx> &ctx) {
    auto left = Interpreter::eval_expr(stmt->m_left.get(), ctx);
    auto right = Interpreter::eval_expr(stmt->m_right.get(), ctx);

    left.value->mutate(right.value);

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj> eval_stmt_while(StmtWhile *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_while");
}

std::shared_ptr<earl::value::Obj> eval_stmt_for(StmtFor *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_for");
}

std::shared_ptr<earl::value::Obj> eval_stmt_class(StmtClass *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_class");
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
    ctx->push_child_context(std::move(child_ctx));
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
