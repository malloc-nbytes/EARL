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

earl::value::Obj *eval_user_defined_closure(earl::variable::Obj *var, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)var;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("eval_user_defined_closure");
}

earl::value::Obj *eval_user_defined_function(earl::function::Obj *func, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)func;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("eval_user_defined_function");
}

earl::value::Obj *eval_user_defined_class_method(earl::function::Obj *method, std::vector<earl::value::Obj *> &params, earl::value::Class *klass, Ctx &ctx, bool _this = false) {
    (void)method;
    (void)params;
    (void)klass;
    (void)ctx;
    (void)_this;
    UNIMPLEMENTED("eval_user_defined_class_method");
}

earl::value::Obj *get_class_member(std::string &id, earl::value::Class *klass, Ctx &ctx, bool _this = false) {
    (void)id;
    (void)klass;
    (void)ctx;
    (void)_this;
    UNIMPLEMENTED("get_class_member");
}

earl::value::Obj *eval_stmt_let(StmtLet *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_let");
}

void load_class_members(StmtLet *stmt, earl::value::Class *klass, Ctx &ctx) {
    (void)stmt;
    (void)klass;
    (void)ctx;
    UNIMPLEMENTED("load_class_members");
}

earl::value::Obj *eval_class_instantiation(ExprFuncCall *expr, Ctx &ctx, bool from_outside) {
    (void)expr;
    (void)ctx;
    (void)from_outside;
    UNIMPLEMENTED("eval_class_instantiation");
}

earl::value::Obj *eval_expr_funccall(ExprFuncCall *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_funccall");
}

earl::value::Obj *eval_expr_list_literal(ExprListLit *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_list_literal");
}

earl::value::Obj *eval_expr_get2(ExprGet *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_get2");
}

earl::value::Obj *eval_expr_get(ExprGet *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_get");
}

earl::value::Obj *eval_expr_array_access(ExprArrayAccess *expr, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    UNIMPLEMENTED("eval_expr_array_access");
}

earl::value::Obj *eval_expr_term(ExprTerm *expr, Ctx &ctx, bool *result_type = nullptr) {
    (void)expr;
    (void)ctx;
    (void)result_type;
    UNIMPLEMENTED("eval_expr_term");
}

earl::value::Obj *eval_expr_bin(ExprBinary *expr, Ctx &ctx, bool *result_type = nullptr) {
    (void)expr;
    (void)ctx;
    (void)result_type;
    UNIMPLEMENTED("eval_expr_bin");
}

earl::value::Obj *Interpreter::eval_expr(Expr *expr, Ctx &ctx, bool *result_type) {
    (void)expr;
    (void)ctx;
    (void)result_type;
    UNIMPLEMENTED("eval_expr");
}

earl::value::Obj *eval_stmt_expr(StmtExpr *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_expr");
}

earl::value::Obj *Interpreter::eval_stmt_block(StmtBlock *block, Ctx &ctx) {
    (void)block;
    (void)ctx;
    UNIMPLEMENTED("Interpreter::eval_stmt_block");
}

earl::value::Obj *eval_stmt_def(StmtDef *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_def");
}

earl::value::Obj *eval_stmt_if(StmtIf *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_if");
}

earl::value::Obj *eval_stmt_return(StmtReturn *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_return");
}

earl::value::Obj *eval_stmt_break(StmtBreak *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_break");
}

earl::value::Obj *eval_stmt_mut(StmtMut *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_mut");
}

earl::value::Obj *eval_stmt_while(StmtWhile *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_while");
}

earl::value::Obj *eval_stmt_for(StmtFor *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_for");
}

earl::value::Obj *eval_stmt_class(StmtClass *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_class");
}

earl::variable::Obj *handle_match_some_branch(ExprFuncCall *expr, earl::value::Obj *inject_value, Ctx &ctx) {
    (void)expr;
    (void)inject_value;
    (void)ctx;
    UNIMPLEMENTED("handle_match_some_branch");
}

earl::value::Obj *eval_stmt_match(StmtMatch *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt_match");
}

earl::value::Obj *eval_stmt(Stmt *stmt, Ctx &ctx) {
    (void)stmt;
    (void)ctx;
    UNIMPLEMENTED("eval_stmt");
}

Ctx *Interpreter::interpret(std::unique_ptr<Program> program, std::unique_ptr<Lexer> lexer) {
    (void)program;
    (void)lexer;
    UNIMPLEMENTED("interpret");
}
