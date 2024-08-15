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

#include <iostream>

#include "err.hpp"
#include "token.hpp"
#include "utils.hpp"

void
Err::err_wtok(Token *tok) {
    std::cerr << tok->m_fp << ':' << tok->m_row << ':' << tok->m_col << '\n';
}

void
Err::err_w2tok(Token *tok1, Token *tok2) {
    std::cerr << tok1->m_fp << ':' << tok1->m_row << ':' << tok1->m_col << '\n';
    std::cerr << tok2->m_fp << ':' << tok2->m_row << ':' << tok2->m_col << '\n';
}

void
Err::err_wconflict(Token *newtok, Token *orig) {
    err_wtok(newtok);
    std::cerr << orig->m_fp << ':' << orig->m_row << ':' << orig->m_col << " <---- conflict\n";
}

void
Err::warn(std::string msg, Token *tok) {
    if (tok) {
        err_wtok(tok);
    }
    std::cerr << "[WARN] " << msg << std::endl;
}

static void err_wident(ExprIdent *expr);

static void
err_wfloatlit(ExprFloatLit *expr) {
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wtuple(ExprTuple *expr) {
    Err::err_wtok(expr->m_tok.get());
    for (auto &e : expr->m_exprs)
        Err::err_wexpr(e.get());
}

static void
err_wclosure(ExprClosure *expr) {
    Err::err_wtok(expr->m_tok.get());
    for (auto &p : expr->m_args)
        Err::err_wtok(p.first.get());
    // TODO: block
}

static void
err_wnone(ExprNone *expr) {
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wbool(ExprBool *expr) {
    Err::err_wtok(expr->m_tok.get());
}

static void
err_warray_access(ExprArrayAccess *expr) {
    Err::err_wtok(expr->m_tok.get());
    Err::err_wexpr(expr->m_left.get());
    Err::err_wexpr(expr->m_expr.get());
}

static void
err_wmod_access(ExprModAccess *expr) {
    Err::err_wtok(expr->m_tok.get());
    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            err_wident(arg.get());
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            err_wfunccall(arg.get());
    }, expr->m_right);
}

static void
err_wget(ExprGet *expr) {
    Err::err_wtok(expr->m_tok.get());
    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            err_wident(arg.get());
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            err_wfunccall(arg.get());
    }, expr->m_right);
}

static void
err_wslice(ExprSlice *expr) {
    Err::err_wtok(expr->m_tok.get());
    if (expr->m_start.has_value())
        Err::err_wexpr(expr->m_start.value().get());
    if (expr->m_end.has_value())
        Err::err_wexpr(expr->m_end.value().get());
}

static void
err_wrange(ExprRange *expr) {
    Err::err_wtok(expr->m_tok.get());
    Err::err_wexpr(expr->m_start.get());
    Err::err_wexpr(expr->m_end.get());
}

static void
err_wlistlit(ExprListLit *expr) {
    Err::err_wtok(expr->m_tok.get());
    for (auto &e : expr->m_elems)
        Err::err_wexpr(e.get());
}

static void
err_wfunccall(ExprFuncCall *expr) {
    Err::err_wtok(expr->m_tok.get());
    Err::err_wexpr(expr->m_left.get());
    for (auto &e : expr->m_params)
        Err::err_wexpr(e.get());
}

static void
err_wcharlit(ExprCharLit *expr) {
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wstrlit(ExprStrLit *expr) {
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wintlit(ExprIntLit *expr) {
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wident(ExprIdent *expr) {
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wterm(ExprTerm *expr) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
        err_wident(dynamic_cast<ExprIdent *>(expr));
    } break;
    case ExprTermType::Int_Literal: {
        err_wintlit(dynamic_cast<ExprIntLit *>(expr));
    } break;
    case ExprTermType::Str_Literal: {
        err_wstrlit(dynamic_cast<ExprStrLit *>(expr));
    } break;
    case ExprTermType::Char_Literal: {
        err_wcharlit(dynamic_cast<ExprCharLit *>(expr));
    } break;
    case ExprTermType::Func_Call: {
        err_wfunccall(dynamic_cast<ExprFuncCall *>(expr));
    } break;
    case ExprTermType::List_Literal: {
        err_wlistlit(dynamic_cast<ExprListLit *>(expr));
    } break;
    case ExprTermType::Range: {
        err_wrange(dynamic_cast<ExprRange *>(expr));
    } break;
    case ExprTermType::Slice: {
        err_wslice(dynamic_cast<ExprSlice *>(expr));
    } break;
    case ExprTermType::Get: {
        err_wget(dynamic_cast<ExprGet *>(expr));
    } break;
    case ExprTermType::Mod_Access: {
        err_wmod_access(dynamic_cast<ExprModAccess *>(expr));
    } break;
    case ExprTermType::Array_Access: {
        err_warray_access(dynamic_cast<ExprArrayAccess *>(expr));
    } break;
    case ExprTermType::Bool: {
        err_wbool(dynamic_cast<ExprBool *>(expr));
    } break;
    case ExprTermType::None: {
        err_wnone(dynamic_cast<ExprNone *>(expr));
    } break;
    case ExprTermType::Closure: {
        err_wclosure(dynamic_cast<ExprClosure *>(expr));
    } break;
    case ExprTermType::Tuple: {
        err_wtuple(dynamic_cast<ExprTuple *>(expr));
    } break;
    case ExprTermType::Float_Literal: {
        err_wfloatlit(dynamic_cast<ExprFloatLit *>(expr));
    } break;
    default: break;
    }
}

static void
err_wbinary(ExprBinary *expr) {
    assert(false);
}

static void
err_wunary(ExprUnary *expr) {
    assert(false);
}

void
Err::err_wexpr(Expr *expr) {
    if (!expr)
        return;
    switch (expr->get_type()) {
    case ExprType::Term: err_wterm(dynamic_cast<ExprTerm *>(expr)); break;
    case ExprType::Binary: err_wbinary(dynamic_cast<ExprBinary *>(expr)); break;
    case ExprType::Unary: err_wunary(dynamic_cast<ExprUnary *>(expr)); break;
    default: break;
    }
}


