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
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "err.hpp"
#include "ast.hpp"
#include "intrinsics.hpp"

struct AutodocCtx {
    std::string src;
    std::vector<std::string> enums;
    std::vector<std::string> classes;
    std::vector<std::string> funcs;
    std::vector<std::string> variables;
};

static std::string
doc_info(std::vector<std::string> &info_lines) {
    std::string s = "";
    for (auto &info : info_lines)
        s += info + "\n";
    return s;
}

static void
stmt_def_doc(StmtDef *stmt, AutodocCtx &ctx) {
    std::string s = "";

    s += "### " + stmt->m_id->lexeme() + "(";

    for (size_t i = 0; i < stmt->m_args.size(); ++i) {
        auto id = stmt->m_args.at(i).first.first.get();
        s += id->lexeme();
        if (i != stmt->m_args.size()-1)
            s += ", ";
    }

    s += ")\n\n";

    s += doc_info(stmt->m_info);

    ctx.funcs.push_back(s);
}

static void
stmt_let_doc(StmtLet *stmt, AutodocCtx &ctx) {
    std::string s = "#### ";

    for (size_t i = 0; i < stmt->m_ids.size(); ++i) {
        s += stmt->m_ids.at(i)->lexeme();
        if (i < stmt->m_ids.size()-1)
            s += ", ";
    }
    s += "\n\n";
    s += doc_info(stmt->m_info);
    ctx.variables.push_back(s);
}

static void
stmt_import_doc(StmtImport *stmt, AutodocCtx &ctx) {
    assert(false);
}

static void
stmt_mod_doc(StmtMod *stmt, AutodocCtx &ctx) {
    ctx.src += "## Module: " + stmt->m_id->lexeme() + "\n\n";
}

static void
stmt_class_doc(StmtClass *stmt, AutodocCtx &ctx) {
    assert(false);
}

static void
stmt_enum_doc(StmtEnum *stmt, AutodocCtx &ctx) {
    assert(false);
}

void
stmt_doc(Stmt *stmt, AutodocCtx &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Def:      stmt_def_doc(dynamic_cast<StmtDef *>(stmt), ctx); break;
    case StmtType::Let:      stmt_let_doc(dynamic_cast<StmtLet *>(stmt), ctx); break;
    case StmtType::Import:   stmt_import_doc(dynamic_cast<StmtImport *>(stmt), ctx); break;
    case StmtType::Mod:      stmt_mod_doc(dynamic_cast<StmtMod *>(stmt), ctx); break;
    case StmtType::Class:    stmt_class_doc(dynamic_cast<StmtClass *>(stmt), ctx); break;
    case StmtType::Enum:     stmt_enum_doc(dynamic_cast<StmtEnum *>(stmt), ctx); break;
    default: return;
    }
}

std::string
get_filename(const std::string& filepath) {
    size_t last_slash = filepath.find_last_of('/');
    if (last_slash != std::string::npos) {
        return filepath.substr(last_slash + 1);
    }
    return filepath;
}

std::string
autodoc(std::unique_ptr<Program> program, std::string &filepath) {
    AutodocCtx ctx;

    ctx.src += "# File: " + get_filename(filepath) + "\n\n";

    for (size_t i = 0; i < program->m_stmts.size(); ++i)
        stmt_doc(program->m_stmts.at(i).get(), ctx);

    std::string res = "";

    res += "## Enums\n\n";
    if (ctx.enums.size() == 0)
        res += "none\n\n";
    for (auto &e : ctx.enums)
        res += e + '\n';

    res += "## Functions\n\n";
    if (ctx.funcs.size() == 0)
        res += "none\n\n";
    for (auto &f : ctx.funcs)
        res += f + '\n';

    res += "## Variables\n\n";
    if (ctx.variables.size() == 0)
        res += "none\n";
    for (auto &v : ctx.variables)
        res += v + '\n';

    return res;
}

