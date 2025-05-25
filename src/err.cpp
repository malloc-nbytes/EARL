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
#include <cassert>

#include "common.hpp"
#include "err.hpp"
#include "token.hpp"

void
Err::err_wtok(Token *tok) {
    if (g_silence_exceptions > 0) return;

    if (!tok)
        return;
    std::cerr << tok->m_fp << ':' << tok->m_row << ':' << tok->m_col << ":\n";

    // Clear artifacts in REPL.
    if ((config::runtime::flags & __REPL) != 0)
        std::cout << "\033[K" << "\r" << std::flush;

    Token *it = tok;
    while (it && it->type() != TokenType::Semicolon) {
        std::cerr << it->lexeme();
        if (it->m_next && it->m_next->type() != TokenType::Semicolon)
            std::cerr << ' ';
        it = it->m_next.get();
    }
    if (it && it->type() == TokenType::Semicolon)
        std::cerr << ';';
    std::cerr << '\n';

    for (int i = 0; i < tok->lexeme().size(); ++i)
        std::cerr << '^';
    std::cerr << std::endl;
}

static void err_wident(ExprIdent *expr, int s);

void
Err::err_w2tok(Token *tok1, Token *tok2) {
    if (g_silence_exceptions > 0) return;
    std::cerr << tok1->m_fp << ':' << tok1->m_row << ':' << tok1->m_col << ":\n";
    std::cerr << tok2->m_fp << ':' << tok2->m_row << ':' << tok2->m_col << ":\n";
}

void
Err::err_wconflict(Token *newtok, Token *orig) {
    if (g_silence_exceptions > 0) return;
    err_wtok(newtok);
    if ((config::runtime::flags & __WATCH) == 0)
        std::cerr << orig->m_fp << ':' << orig->m_row << ':' << orig->m_col << ": <---- conflict\n";
}

void
Err::warn(std::string msg, Token *tok) {
    if (tok)
        err_wtok(tok);
    std::cerr << "warning: " << msg << std::endl;
}

static void
err_wfloatlit(const ExprFloatLit *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wtuple(const ExprTuple *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wclosure(const ExprClosure *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wnone(const ExprNone *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wbool(const ExprBool *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_warray_access(const ExprArrayAccess *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wmod_access(const ExprModAccess *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wget(const ExprGet *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wslice(const ExprSlice *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wrange(const ExprRange *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wlistlit(const ExprListLit *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wfunccall(const ExprFuncCall *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wcharlit(const ExprCharLit *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wstrlit(const ExprStrLit *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wintlit(const ExprIntLit *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wident(const ExprIdent *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wfstr(const ExprFStr *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(expr->m_tok.get());
}

static void
err_wcase(const ExprCase *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    Err::err_wexpr(expr->m_expr.get());
}

static void
err_wterm(const ExprTerm *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    switch (expr->get_term_type()) {
    case ExprTermType::Ident: {
        err_wident(dynamic_cast<const ExprIdent *>(expr), s);
    } break;
    case ExprTermType::Int_Literal: {
        err_wintlit(dynamic_cast<const ExprIntLit *>(expr), s);
    } break;
    case ExprTermType::Str_Literal: {
        err_wstrlit(dynamic_cast<const ExprStrLit *>(expr), s);
    } break;
    case ExprTermType::Char_Literal: {
        err_wcharlit(dynamic_cast<const ExprCharLit *>(expr), s);
    } break;
    case ExprTermType::Func_Call: {
        err_wfunccall(dynamic_cast<const ExprFuncCall *>(expr), s);
    } break;
    case ExprTermType::List_Literal: {
        err_wlistlit(dynamic_cast<const ExprListLit *>(expr), s);
    } break;
    case ExprTermType::Range: {
        err_wrange(dynamic_cast<const ExprRange *>(expr), s);
    } break;
    case ExprTermType::Slice: {
        err_wslice(dynamic_cast<const ExprSlice *>(expr), s);
    } break;
    case ExprTermType::Get: {
        err_wget(dynamic_cast<const ExprGet *>(expr), s);
    } break;
    case ExprTermType::Mod_Access: {
        err_wmod_access(dynamic_cast<const ExprModAccess *>(expr), s);
    } break;
    case ExprTermType::Array_Access: {
        err_warray_access(dynamic_cast<const ExprArrayAccess *>(expr), s);
    } break;
    case ExprTermType::Bool: {
        err_wbool(dynamic_cast<const ExprBool *>(expr), s);
    } break;
    case ExprTermType::None: {
        err_wnone(dynamic_cast<const ExprNone *>(expr), s);
    } break;
    case ExprTermType::Closure: {
        err_wclosure(dynamic_cast<const ExprClosure *>(expr), s);
    } break;
    case ExprTermType::Tuple: {
        err_wtuple(dynamic_cast<const ExprTuple *>(expr), s);
    } break;
    case ExprTermType::Float_Literal: {
        err_wfloatlit(dynamic_cast<const ExprFloatLit *>(expr), s);
    } break;
    case ExprTermType::FStr: {
        err_wfstr(dynamic_cast<const ExprFStr *>(expr), s);
    } break;
    case ExprTermType::Case: {
        err_wcase(dynamic_cast<const ExprCase *>(expr), s);
    } break;
    default: break;
    }
}

static void
err_wbinary(const ExprBinary *const expr) {
    if (g_silence_exceptions > 0) return;
    Err::err_wexpr(expr->m_lhs.get());
    Err::err_wexpr(expr->m_rhs.get());
}

static void
err_wunary(const ExprUnary *const expr) {
    if (g_silence_exceptions > 0) return;
    Err::err_wexpr(expr->m_expr.get());
}

void
Err::err_wexpr(const Expr *const expr, int s) {
    if (g_silence_exceptions > 0) return;
    if (!expr) {
        return;
    }
    switch (expr->get_type()) {
    case ExprType::Term: err_wterm(dynamic_cast<const ExprTerm *>(expr), s); break;
    case ExprType::Binary: err_wbinary(dynamic_cast<const ExprBinary *>(expr)); break;
    case ExprType::Unary: err_wunary(dynamic_cast<const ExprUnary *>(expr)); break;
    default: break;
    }
}

void
err_wdefstmt(StmtDef *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(stmt->m_id.get());
}

void
err_wletstmt(StmtLet *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wblockstmt(StmtBlock *stmt) {
    if (g_silence_exceptions > 0) return;
    for (auto &s : stmt->m_stmts) {
        Err::err_wstmt(s.get());
    }
}

void
err_wmutstmt(StmtMut *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wexpr(stmt->m_left.get());
    Err::err_wtok(stmt->m_equals.get());
    Err::err_wexpr(stmt->m_right.get());
}

void
err_wstmtexpr(StmtExpr *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wexpr(stmt->m_expr.get());
}

void
err_wstmtif(StmtIf *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wstmt(stmt->m_block.get());
}

void
err_wstmtreturn(StmtReturn *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(stmt->m_tok.get());
}

void
err_wstmtbreak(StmtBreak *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtwhile(StmtWhile *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtfor(StmtFor *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtforeach(StmtForeach *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtimport(StmtImport *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtmod(StmtMod *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtclass(StmtClass *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtmatch(StmtMatch *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wstmtenum(StmtEnum *stmt) {
    if (g_silence_exceptions > 0) return;
    assert(false);
}

void
err_wimportstmt_stmt(StmtImport *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wexpr(stmt->m_fp.get());
}

void
err_wexec_stmt(StmtExec *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wtok(stmt->m_ident.get());
}

void
err_wuse_stmt(StmtUse *stmt) {
    if (g_silence_exceptions > 0) return;
    Err::err_wexpr(stmt->m_fp.get());
}

void
Err::err_wstmt(Stmt *stmt) {
    if (g_silence_exceptions > 0) return;
    if (!stmt) {
        std::cerr << "[EARL] <unable to display error line at this time>" << std::endl;;
        return;
    }
    switch (stmt->stmt_type()) {
    case StmtType::Def: err_wdefstmt(dynamic_cast<StmtDef *>(stmt)); break;
    case StmtType::Let: assert(false); break;
    case StmtType::Block: err_wblockstmt(dynamic_cast<StmtBlock *>(stmt)); break;
    case StmtType::Mut: err_wmutstmt(dynamic_cast<StmtMut *>(stmt)); break;
    case StmtType::Stmt_Expr: err_wstmtexpr(dynamic_cast<StmtExpr *>(stmt)); break;
    case StmtType::If: err_wstmtif(dynamic_cast<StmtIf *>(stmt)); break;
    case StmtType::Return: err_wstmtreturn(dynamic_cast<StmtReturn *>(stmt)); break;
    case StmtType::Break: assert(false); break;
    case StmtType::While: assert(false); break;
    case StmtType::For: assert(false); break;
    case StmtType::Foreach: assert(false); break;
    case StmtType::Import: err_wimportstmt_stmt(dynamic_cast<StmtImport *>(stmt)); break;
    case StmtType::Mod: assert(false); break;
    case StmtType::Class: assert(false); break;
    case StmtType::Match: assert(false); break;
    case StmtType::Enum: assert(false); break;
    case StmtType::Use: err_wuse_stmt(dynamic_cast<StmtUse *>(stmt)); break;
    case StmtType::Exec: err_wexec_stmt(dynamic_cast<StmtExec *>(stmt)); break;
    default:
        assert(false && "unreachable");
    }
}
