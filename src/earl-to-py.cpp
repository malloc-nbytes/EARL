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

// *** TODO: Update bitwise binary operators for Python ***

#include <iostream>
#include <cassert>

#include "err.hpp"
#include "ast.hpp"

#define PYSTMT(s, ctx)                          \
    do {                                        \
        tabs(ctx);                              \
        ctx.py_src += s "\n";                   \
    } while (0)

#define PYAPPEND(s, ctx)                                                \
    do {                                                                \
        ctx.py_src += s;                                                \
    } while (0)

#define PYSTMT_CONS(s, ctx)                     \
    do {                                        \
        tabs(ctx);                              \
        ctx.py_src += s+"\n";                   \
    } while (0)

#define PYTAB "    "

struct Context {
    unsigned scope_depth;
    std::string py_src;
};

static std::string expr_to_py(Expr *expr, Context &ctx);
static void stmt_block_to_py(StmtBlock *stmt, Context &ctx);
static void stmt_to_py(Stmt *stmt, Context &ctx);

void tabs(Context &ctx) {
    for (unsigned i = 0; i < ctx.scope_depth; ++i) {
        ctx.py_src += PYTAB;
    }
}

static std::string
expr_term_ident_to_py(ExprIdent *expr, Context &ctx) {
    return expr->m_tok->lexeme();
}

static std::string
expr_term_intlit_to_py(ExprIntLit *expr, Context &ctx) {
    return expr->m_tok->lexeme();
}

static std::string
expr_term_strlit_to_py(ExprStrLit *expr, Context &ctx) {
    return "\"" + expr->m_tok->lexeme() + "\"";
}

static std::string
expr_term_charlit_to_py(ExprCharLit *expr, Context &ctx)  {
    return "'" + expr->m_tok->lexeme() + "'";
}

static std::string
expr_term_floatlit_to_py(ExprFloatLit *expr, Context &ctx)  {
}

static std::string
expr_term_funccall_to_py(ExprFuncCall *expr, Context &ctx) {
    std::string accessor = expr_to_py(expr->m_left.get(), ctx);
    std::string params = "";
    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        params += expr_to_py(expr->m_params.at(i).get(), ctx);
        if (i != expr->m_params.size() - 1)
            params += ", ";
    }
    return accessor+"("+params+")";
}

static std::string
expr_term_listlit_to_py(ExprListLit *expr, Context &ctx) {
    std::string exprs = "[";
    for (size_t i = 0; i < expr->m_elems.size(); ++i) {
        exprs += expr_to_py(expr->m_elems.at(i).get(), ctx);
        if (i != expr->m_elems.size() - 1)
            exprs += ", ";
    }
    return exprs+"]";
}

static std::string
expr_term_get_to_py(ExprGet *expr, Context &ctx) {
    std::string left = expr_to_py(expr->m_left.get(), ctx);
    std::string right = "";

    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            right = expr_to_py(arg.get(), ctx);
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            right = expr_to_py(arg.get(), ctx);
        else {
            std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
            throw InterpreterException(msg);
        }
    }, expr->m_right);

    return left+"."+right;
}

static std::string
expr_term_mod_to_py_access(ExprModAccess *expr, Context &ctx) {
}

static std::string
expr_term_array_to_py_access(ExprArrayAccess *expr, Context &ctx) {
}

static std::string
expr_term_boollit_to_py(ExprBool *expr, Context &context)  {
}

static std::string
expr_term_none_to_py(ExprNone *expr, Context &context)  {
}

static std::string
expr_term_closure_to_py(ExprClosure *expr, Context &ctx) {
}

static std::string
expr_term_range_to_py(ExprRange *expr, Context &ctx) {
}

static std::string
expr_term_tuple_to_py(ExprTuple *expr, Context &ctx) {
}

static std::string
expr_term_slice_to_py(ExprSlice *expr, Context &ctx) {
}

static std::string
expr_term_dict_to_py(ExprDict *expr, Context &ctx) {
}

static std::string
expr_term_fstr_to_py(ExprFStr *expr, Context &ctx) {
}

static std::string
expr_term_to_py(ExprTerm *expr, Context &ctx) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident:         return expr_term_ident_to_py(dynamic_cast<ExprIdent *>(expr), ctx);
    case ExprTermType::Int_Literal:   return expr_term_intlit_to_py(dynamic_cast<ExprIntLit *>(expr), ctx);
    case ExprTermType::Str_Literal:   return expr_term_strlit_to_py(dynamic_cast<ExprStrLit *>(expr), ctx);
    case ExprTermType::Char_Literal:  return expr_term_charlit_to_py(dynamic_cast<ExprCharLit *>(expr), ctx);
    case ExprTermType::Float_Literal: return expr_term_floatlit_to_py(dynamic_cast<ExprFloatLit *>(expr), ctx);
    case ExprTermType::Func_Call:     return expr_term_funccall_to_py(dynamic_cast<ExprFuncCall *>(expr), ctx);
    case ExprTermType::List_Literal:  return expr_term_listlit_to_py(dynamic_cast<ExprListLit *>(expr), ctx);
    case ExprTermType::Get:           return expr_term_get_to_py(dynamic_cast<ExprGet *>(expr), ctx);
    case ExprTermType::Mod_Access:    return expr_term_mod_to_py_access(dynamic_cast<ExprModAccess *>(expr), ctx);
    case ExprTermType::Array_Access:  return expr_term_array_to_py_access(dynamic_cast<ExprArrayAccess *>(expr), ctx);
    case ExprTermType::Bool:          return expr_term_boollit_to_py(dynamic_cast<ExprBool *>(expr), ctx);
    case ExprTermType::None:          return expr_term_none_to_py(dynamic_cast<ExprNone *>(expr), ctx);
    case ExprTermType::Closure:       return expr_term_closure_to_py(dynamic_cast<ExprClosure *>(expr), ctx);
    case ExprTermType::Range:         return expr_term_range_to_py(dynamic_cast<ExprRange *>(expr), ctx);
    case ExprTermType::Tuple:         return expr_term_tuple_to_py(dynamic_cast<ExprTuple *>(expr), ctx);
    case ExprTermType::Slice:         return expr_term_slice_to_py(dynamic_cast<ExprSlice *>(expr), ctx);
    case ExprTermType::Dict:          return expr_term_dict_to_py(dynamic_cast<ExprDict *>(expr), ctx);
    case ExprTermType::FStr:          return expr_term_fstr_to_py(dynamic_cast<ExprFStr *>(expr), ctx);
    default: {
        std::string msg = "unknown term: `"+std::to_string((int)expr->get_term_type())+"`";
        throw InterpreterException(msg);
    }
    }
}

static std::string
expr_bin_to_py(ExprBinary *expr, Context &ctx) {
    std::string lhs = expr_to_py(expr->m_lhs.get(), ctx);
    std::string rhs = expr_to_py(expr->m_rhs.get(), ctx);
    return lhs+" "+expr->m_op->lexeme()+" "+rhs;
}

static std::string
expr_unary_to_py(ExprUnary *expr, Context &ctx) {
    std::string pyexpr = expr_to_py(expr->m_expr.get(), ctx);
    return expr->m_op->lexeme()+pyexpr;
}

static std::string
expr_to_py(Expr *expr, Context &ctx) {
    switch (expr->get_type()) {
    case ExprType::Term: {
        return expr_term_to_py(dynamic_cast<ExprTerm *>(expr), ctx);
    } break;
    case ExprType::Binary: {
        return expr_bin_to_py(dynamic_cast<ExprBinary *>(expr), ctx);
    } break;
    case ExprType::Unary: {
        return expr_unary_to_py(dynamic_cast<ExprUnary *>(expr), ctx);
    } break;
    default: assert(false && "unreachable"); break;
    }
}

static void
stmt_def_to_py(StmtDef *stmt, Context &ctx) {
    const std::string &id = stmt->m_id->lexeme();
    std::string params = "";
    for (size_t i = 0; i < stmt->m_args.size(); ++i) {
        params += stmt->m_args.at(i).first.first->lexeme();
        if (i != stmt->m_args.size() - 1)
            params += ", ";
    }
    PYSTMT_CONS("def "+id+"("+params+"):", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_let_to_py(StmtLet *stmt, Context &ctx) {
    if (stmt->m_ids.size() > 1)
        ERR(Err::Type::Fatal, "multiple ids in `let` statement is unimplemented");
    const std::string &id = stmt->m_ids.at(0)->lexeme();
    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS(id + " = " + pyexpr, ctx);
}

static void
stmt_block_to_py(StmtBlock *stmt, Context &ctx) {
    ctx.scope_depth++;

    if (stmt->m_stmts.size() == 0)
        PYSTMT("pass", ctx);

    for (size_t i = 0; i < stmt->m_stmts.size(); ++i)
        stmt_to_py(stmt->m_stmts.at(i).get(), ctx);

    ctx.scope_depth--;
}

static void
stmt_mut_to_py(StmtMut *stmt, Context &ctx) {
    const std::string pyleft = expr_to_py(stmt->m_left.get(), ctx);
    const std::string pyright = expr_to_py(stmt->m_right.get(), ctx);
    PYSTMT_CONS(pyleft+" "+stmt->m_equals->lexeme()+" "+pyright, ctx);
}

static void
stmt_expr_to_py(StmtExpr *stmt, Context &ctx) {
    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS(pyexpr, ctx);
}

static void
stmt_if_to_py(StmtIf *stmt, Context &ctx) {
    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS("if "+pyexpr+":", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
    if (stmt->m_else.has_value()) {
        PYSTMT("else:", ctx);
        stmt_block_to_py(stmt->m_else.value().get(), ctx);
    }
}

static void
stmt_return_to_py(StmtReturn *stmt, Context &ctx) {
    std::string pyexpr = "";
    if (stmt->m_expr.has_value())
        pyexpr += " " + expr_to_py(stmt->m_expr.value().get(), ctx);

    PYSTMT_CONS("return"+pyexpr, ctx);
}

static void
stmt_break_to_py(StmtBreak *stmt, Context &ctx) {
    PYSTMT("break", ctx);
}

static void
stmt_while_to_py(StmtWhile *stmt, Context &ctx) {
    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS("while "+pyexpr+":", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_foreach_to_py(StmtForeach *stmt, Context &ctx) {
}

static void
stmt_for_to_py(StmtFor *stmt, Context &ctx) {
    std::string enumerator = stmt->m_enumerator->lexeme();
    std::string start = expr_to_py(stmt->m_start.get(), ctx);
    std::string end = expr_to_py(stmt->m_end.get(), ctx);
    std::string range = "range("+start+", "+end+")";
    PYSTMT_CONS("for " + enumerator + " in " + range + ":", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_import_to_py(StmtImport *stmt, Context &ctx) {
}

static void
stmt_mod_to_py(StmtMod *stmt, Context &ctx) {
    (void)stmt;
    (void)ctx;
    // nothing to do...
}

static void
stmt_class_to_py(StmtClass *stmt, Context &ctx) {
}

static void
stmt_match_to_py(StmtMatch *stmt, Context &ctx) {
}

static void
stmt_enum_to_py(StmtEnum *stmt, Context &ctx) {
}

static void
stmt_continue_to_py(StmtContinue *stmt, Context &ctx) {
}

static void
stmt_loop_to_py(StmtLoop *stmt, Context &ctx) {
    PYSTMT("while True:", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_to_py(Stmt *stmt, Context &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Def:      stmt_def_to_py(dynamic_cast<StmtDef *>(stmt), ctx); break;
    case StmtType::Let:      stmt_let_to_py(dynamic_cast<StmtLet *>(stmt), ctx); break;
    case StmtType::Block:    stmt_block_to_py(dynamic_cast<StmtBlock *>(stmt), ctx); break;
    case StmtType::Mut:      stmt_mut_to_py(dynamic_cast<StmtMut *>(stmt), ctx); break;
    case StmtType::Stmt_Expr:stmt_expr_to_py(dynamic_cast<StmtExpr *>(stmt), ctx); break;
    case StmtType::If:       stmt_if_to_py(dynamic_cast<StmtIf *>(stmt), ctx); break;
    case StmtType::Return:   stmt_return_to_py(dynamic_cast<StmtReturn *>(stmt), ctx); break;
    case StmtType::Break:    stmt_break_to_py(dynamic_cast<StmtBreak *>(stmt), ctx); break;
    case StmtType::While:    stmt_while_to_py(dynamic_cast<StmtWhile *>(stmt), ctx); break;
    case StmtType::Foreach:  stmt_foreach_to_py(dynamic_cast<StmtForeach *>(stmt), ctx); break;
    case StmtType::For:      stmt_for_to_py(dynamic_cast<StmtFor *>(stmt), ctx); break;
    case StmtType::Import:   stmt_import_to_py(dynamic_cast<StmtImport *>(stmt), ctx); break;
    case StmtType::Mod:      stmt_mod_to_py(dynamic_cast<StmtMod *>(stmt), ctx); break;
    case StmtType::Class:    stmt_class_to_py(dynamic_cast<StmtClass *>(stmt), ctx); break;
    case StmtType::Match:    stmt_match_to_py(dynamic_cast<StmtMatch *>(stmt), ctx); break;
    case StmtType::Enum:     stmt_enum_to_py(dynamic_cast<StmtEnum *>(stmt), ctx); break;
    case StmtType::Continue: stmt_continue_to_py(dynamic_cast<StmtContinue *>(stmt), ctx); break;
    case StmtType::Loop:     stmt_loop_to_py(dynamic_cast<StmtLoop *>(stmt), ctx); break;
    default: assert(false && "unreachable");
    }
}

std::string
earl_to_py(std::unique_ptr<Program> program) {
    Context ctx = {0, ""};
    for (size_t i = 0; i < program->m_stmts.size(); ++i) {
        stmt_to_py(program->m_stmts.at(i).get(), ctx);
    }
    return ctx.py_src;
}

