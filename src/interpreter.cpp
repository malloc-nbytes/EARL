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

earl::value::Obj *eval_stmt(Stmt *stmt, Ctx &ctx);
earl::value::Obj *eval_stmt_block(StmtBlock *block, Ctx &ctx);

earl::value::Obj *eval_user_defined_function(earl::function::Obj *func, std::vector<earl::value::Obj *> params, Ctx &ctx) {
    ctx.set_function(func);
    func->load_parameters(params);

    func->debug_dump();

    earl::value::Obj *result = eval_stmt_block(func->block(), ctx);
    ctx.unset_function();
    return result;
}

earl::value::Obj *eval_expr_module_funccall(ExprFuncCall *expr, Ctx &main_ctx, Ctx &mod_ctx) {
    std::vector<earl::value::Obj *> params;
    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        params.push_back(Interpreter::eval_expr(expr->m_params.at(i).get(), main_ctx));
    }

    earl::function::Obj *func = mod_ctx.get_registered_function(expr->m_id->lexeme());

    return eval_user_defined_function(func, params, mod_ctx);
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

earl::value::Obj *eval_expr_list_literal(ExprListLit *expr, Ctx &ctx) {
    std::vector<earl::value::Obj *> list;
    for (size_t i = 0; i < expr->m_elems.size(); ++i) {
        list.push_back(Interpreter::eval_expr(expr->m_elems.at(i).get(), ctx));
    }
    return new earl::value::List(std::move(list));
}

earl::value::Obj *eval_expr_get(ExprGet *expr, Ctx &ctx) {
    earl::value::Obj *result = nullptr;

    earl::value::Obj *left = Interpreter::eval_expr(expr->m_left.get(), ctx);

    if (expr->get_type() != ExprType::Term) {
        ERR(Err::Type::Fatal, "cannot use `get` expression on non-terminal expression");
    }

    ExprTerm *right = dynamic_cast<ExprTerm *>(expr->m_right.get());

    switch (right->get_term_type()) {
    case ExprTermType::Ident: {
        UNIMPLEMENTED("eval_expr_get::ExprTermType::Ident");
    } break;
    case ExprTermType::Func_Call: {
        ExprFuncCall *func_expr = dynamic_cast<ExprFuncCall *>(right);
        const std::string &id = func_expr->m_id->lexeme();
        std::vector<earl::value::Obj *> params;

        std::for_each(func_expr->m_params.begin(), func_expr->m_params.end(), [&](auto &e) {
            params.push_back(Interpreter::eval_expr(e.get(), ctx));
        });

        if (left->type() == earl::value::Type::Module) {
            auto mod = dynamic_cast<earl::value::Module *>(left);
            // return eval_expr_funccall(func_expr, *mod->value());
            return eval_expr_module_funccall(func_expr, ctx, *mod->value());
        }
        else if (Intrinsics::is_member_intrinsic(id)) {
            result = Intrinsics::call_member(id, left, params, ctx);
        }
        else {
            UNIMPLEMENTED("eval_expr_get:ExprTermType::Func_Call:!Intrinsics::is_member_intrinsic(id)");
        }

    } break;
    default: {
        ERR_WARGS(Err::Type::Fatal,
                  "unknown `get` term type (%d)",
                  static_cast<int>(right->get_term_type()));
    } break;
    }

    return result;
}

earl::value::Obj *eval_expr_term(ExprTerm *expr, Ctx &ctx) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {

        ExprIdent *ident = dynamic_cast<ExprIdent *>(expr);

        // Check for a module
        earl::value::Module *mod = ctx.get_registered_module(ident->m_tok->lexeme());
        if (mod)
            return mod;

        // Not a module, find the variable
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
    case ExprTermType::List_Literal: {
        return eval_expr_list_literal(dynamic_cast<ExprListLit *>(expr), ctx);
    } break;
    case ExprTermType::Get: {
        ExprGet *get = dynamic_cast<ExprGet *>(expr);
        return eval_expr_get(get, ctx);
    }
    default: {
        ERR_WARGS(Err::Type::Fatal, "unknown expression term type %d", static_cast<int>(expr->get_term_type()));
    } break;
    }
}

earl::value::Obj *eval_expr_bin(ExprBinary *expr, Ctx &ctx) {
    earl::value::Obj *lhs = Interpreter::eval_expr(expr->m_lhs.get(), ctx);
    earl::value::Obj *rhs = Interpreter::eval_expr(expr->m_rhs.get(), ctx);

    earl::value::Obj *result = lhs->binop(expr->m_op.get(), rhs);

    return result;
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

    earl::value::Obj *rhs_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);

    // NOTE: If we want to take a 'reference' to `rhs_result`, instead of
    //     (stmt->m_id.get(), std::unique_ptr<earl::value::Obj>(rhs_result->copy()))
    // we would instead use:
    //     (stmt->m_id.get(), std::unique_ptr<earl::value::Obj>(rhs_result))
    earl::variable::Obj *created_variable
        = new earl::variable::Obj(stmt->m_id.get(), std::unique_ptr<earl::value::Obj>(rhs_result->copy()));

    ctx.register_variable(created_variable);

    return new earl::value::Void();
}

earl::value::Obj *eval_stmt_expr(StmtExpr *stmt, Ctx &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx);
}

earl::value::Obj *eval_stmt_block(StmtBlock *block, Ctx &ctx) {
    earl::value::Obj *result = nullptr;

    ctx.push_scope();
    for (size_t i = 0; i < block->m_stmts.size(); ++i) {
        result = eval_stmt(block->m_stmts.at(i).get(), ctx);
        if (result && result->type() != earl::value::Type::Void)
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
earl::value::Obj *eval_stmt_def(StmtDef *stmt, Ctx &ctx) {
    if (ctx.function_is_registered(stmt->m_id->lexeme())) {
        ERR_WARGS(Err::Type::Redeclared,
                  "function `%s` is already declared", stmt->m_id->lexeme().c_str());
    }

    // for (size_t i = 0; i < stmt->m_args.size(); ++i) {
    //     std::unique_ptr<Token> tok = std::move(stmt->m_args[i]);
        // params.push_back(std::move(list));
    // }

    earl::function::Obj *created_function = new earl::function::Obj(stmt, std::move(stmt->m_args));
    ctx.register_function(created_function);
    return new earl::value::Void();
}

earl::value::Obj *eval_stmt_if(StmtIf *stmt, Ctx &ctx) {
    earl::value::Obj *expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx);
    earl::value::Obj *result = nullptr;

    if (expr_result->boolean()) {
        result = eval_stmt_block(stmt->m_block.get(), ctx);
    }
    else if (stmt->m_else.has_value()) {
        result = eval_stmt_block(stmt->m_else.value().get(), ctx);
    }

    delete expr_result;

    return result;
}

earl::value::Obj *eval_stmt_return(StmtReturn *stmt, Ctx &ctx) {
    return Interpreter::eval_expr(stmt->m_expr.get(), ctx);
}

earl::value::Obj *eval_stmt_mut(StmtMut *stmt, Ctx &ctx) {
    earl::value::Obj *left = Interpreter::eval_expr(stmt->m_left.get(), ctx);
    earl::value::Obj *right = Interpreter::eval_expr(stmt->m_right.get(), ctx);

    left->mutate(right);

    return new earl::value::Void();
}

earl::value::Obj *eval_stmt_while(StmtWhile *stmt, Ctx &ctx) {
    earl::value::Obj *expr_result = nullptr;
    earl::value::Obj *result = nullptr;

    while ((expr_result = Interpreter::eval_expr(stmt->m_expr.get(), ctx))->boolean()) {
        result = eval_stmt_block(stmt->m_block.get(), ctx);
        if (result && result->type() != earl::value::Type::Void)
            break;
        delete expr_result;
    }

    return result;
}

earl::value::Obj *eval_stmt_for(StmtFor *stmt, Ctx &ctx) {
    earl::value::Obj *result = nullptr;
    earl::value::Obj *start_expr = Interpreter::eval_expr(stmt->m_start.get(), ctx);
    earl::value::Obj *end_expr = Interpreter::eval_expr(stmt->m_end.get(), ctx);

    earl::variable::Obj *enumerator
        = new earl::variable::Obj(stmt->m_enumerator.get(), std::unique_ptr<earl::value::Obj>(start_expr));

    assert(!ctx.variable_is_registered(enumerator->id()));
    ctx.register_variable(enumerator);

    earl::value::Int *start = dynamic_cast<earl::value::Int *>(start_expr);
    earl::value::Int *end = dynamic_cast<earl::value::Int *>(end_expr);

    while (start->value() < end->value()) {
        result = eval_stmt_block(stmt->m_block.get(), ctx);

        if (result && result->type() != earl::value::Type::Void)
            break;

        start->mutate(new earl::value::Int(start->value() + 1));
    }

    ctx.unregister_variable(enumerator->id());

    delete start_expr;
    delete end_expr;

    return result;
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
    case StmtType::Mod: {
        StmtMod *mod = dynamic_cast<StmtMod *>(stmt);
        ctx.set_module(std::move(mod->m_id));
        return new earl::value::Void();
    } break;
    case StmtType::Import: {
        StmtImport *im = dynamic_cast<StmtImport *>(stmt);

        std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
        std::vector<std::string> types = COMMON_EARLTY_ASCPL;
        std::string comment = COMMON_EARL_COMMENT;

        std::unique_ptr<Lexer> lexer = lex_file(im->m_fp.get()->lexeme().c_str(), keywords, types, comment);
        std::unique_ptr<Program> program = Parser::parse_program(*lexer.get());
        Ctx *child_ctx = Interpreter::interpret(std::move(program), std::move(lexer));

        ctx.push_child_context(std::unique_ptr<Ctx>(std::move(child_ctx)));

        return new earl::value::Void();
    } break;
    default:
        assert(false && "eval_stmt: invalid statement");
    }
}

Ctx *Interpreter::interpret(std::unique_ptr<Program> program, std::unique_ptr<Lexer> lexer) {
    Ctx *ctx = new Ctx(std::move(lexer), std::move(program));
    earl::value::Obj *meta;

    for (size_t i = 0; i < ctx->stmts_len(); ++i) {
        meta = eval_stmt(ctx->get_stmt(i), *ctx);
        delete meta;
    }

    return std::move(ctx);
}
