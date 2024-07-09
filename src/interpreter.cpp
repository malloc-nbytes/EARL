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

static Ctx *parent_ctx = nullptr;

earl::value::Obj *eval_stmt(Stmt *stmt, Ctx &ctx);

std::shared_ptr<earl::value::Obj> eval_user_defined_closure(std::shared_ptr<earl::variable::Obj> var, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)var;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("eval_user_defined_closure");
}

std::shared_ptr<earl::value::Obj> eval_user_defined_function(std::shared_ptr<earl::function::Obj> func, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)func;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("eval_user_defined_function");
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
    auto var = std::make_shared<earl::variable::Obj>(stmt->m_id.get(), val, stmt->m_attrs);

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

std::shared_ptr<earl::value::Obj> eval_expr_term(ExprTerm *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_term");
}

std::shared_ptr<earl::value::Obj> eval_expr_bin(ExprBinary *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_bin");
}

std::shared_ptr<earl::value::Obj> Interpreter::eval_expr(Expr *expr, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr");
}

std::shared_ptr<earl::value::Obj> eval_stmt_expr(StmtExpr *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_expr");
}

std::shared_ptr<earl::value::Obj> Interpreter::eval_stmt_block(StmtBlock *block, std::shared_ptr<Ctx> &ctx) {
    (void)block;
    (void)ctx;
    UNIMPLEMENTED("Interpreter::eval_stmt_block");
}

std::shared_ptr<earl::value::Obj> eval_stmt_def(StmtDef *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_def");
}

std::shared_ptr<earl::value::Obj> eval_stmt_if(StmtIf *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_if");
}

std::shared_ptr<earl::value::Obj> eval_stmt_return(StmtReturn *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_return");
}

std::shared_ptr<earl::value::Obj> eval_stmt_break(StmtBreak *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_break");
}

std::shared_ptr<earl::value::Obj> eval_stmt_mut(StmtMut *stmt, std::shared_ptr<Ctx> &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_mut");
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
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_import");
}

std::shared_ptr<earl::value::Obj> eval_stmt(Stmt *stmt, std::shared_ptr<Ctx> &ctx) {
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
    std::shared_ptr<Ctx> ctx = std::make_shared<Ctx>(std::move(lexer), std::move(program));

    for (size_t i = 0; i < ctx->stmts_len(); ++i) {
        auto meta = eval_stmt(ctx->get_stmt_at(i), ctx);
        (void)meta;
    }

    return ctx;
}
