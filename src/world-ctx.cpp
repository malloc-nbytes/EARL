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
#include <iostream>

#include "ctx.hpp"
#include "utils.hpp"
#include "err.hpp"
#include "common.hpp"

WorldCtx::WorldCtx(std::unique_ptr<Lexer> lexer, std::unique_ptr<Program> program)
    : m_lexer(std::move(lexer)), m_program(std::move(program)) {
    m_filepath = m_program->m_filepath;
}

WorldCtx::WorldCtx() : m_lexer(nullptr), m_program(nullptr) {}

void
WorldCtx::add_external_shell_script(std::shared_ptr<Token> as, std::string path, Expr *expr) {
    auto already_has = m_external_bash_scripts.find(as->lexeme());
    if (already_has != m_external_bash_scripts.end()) {
        Err::err_wexpr(expr);
        const std::string msg = "external script alias "+as->lexeme()+"already exists";
        throw InterpreterException(msg);
    }
    m_external_bash_scripts[as->lexeme()] = std::move(path);
}

const std::string &
WorldCtx::get_external_script_path(const std::string &alias, Stmt *stmt) {
    auto it = m_external_bash_scripts.find(alias);
    if (it == m_external_bash_scripts.end()) {
        Err::err_wstmt(stmt);
        const std::string msg = "script for alias `"+alias+"` does not exist";
        throw InterpreterException(msg);
    }
    return it->second;
}

void
WorldCtx::set_module_alias(const std::string &id) {
    m_module_alias = id;
}

Program *
WorldCtx::get_repl_program(size_t i) {
    if (i >= m_repl_programs.size())
        return nullptr;
    return m_repl_programs.at(i).get();
}

size_t
WorldCtx::get_repl_programs_len(void) const {
    return m_repl_programs.size();
}

void
WorldCtx::add_repl_lexer(std::unique_ptr<Lexer> lexer) {
    m_repl_lexers.push_back(std::move(lexer));
}

void
WorldCtx::add_repl_program(std::unique_ptr<Program> program) {
    m_repl_programs.push_back(std::move(program));
}

size_t
WorldCtx::stmts_len(void) const {
    if (!m_program) {
        if (m_repl_programs.size() > 0)
            return m_repl_programs.back()->m_stmts.size();
        else
            return 0;
    }
    return m_program->m_stmts.size();
}

Stmt *
WorldCtx::stmt_at(size_t idx) {
    if (!m_program)
        return m_repl_programs.back()->m_stmts.at(idx).get();
    return m_program->m_stmts.at(idx).get();
}

void
WorldCtx::set_mod(std::string id) {
    m_mod = id;
}

void
WorldCtx::add_import(std::shared_ptr<Ctx> ctx) {
    m_imports.push_back(std::move(ctx));
}

const std::string &
WorldCtx::get_mod(void) const {
    return m_mod;
}

bool
WorldCtx::import_is_defined(const std::string &id) {
    for (auto &im : m_imports)
        if (dynamic_cast<WorldCtx *>(im.get())->get_mod() == id)
            return true;
    return false;
}

bool
WorldCtx::has_module_alias(void) const {
    return m_module_alias.has_value();
}

const std::string &
WorldCtx::get_module_alias(void) const {
    return m_module_alias.value();
}

std::shared_ptr<Ctx> *
WorldCtx::get_import(const std::string &id) {
    for (auto &im : m_imports) {
        auto wctx = dynamic_cast<WorldCtx *>(im.get());
        if (wctx->get_mod() == id)
            return &im;
        if (wctx->has_module_alias() && wctx->get_module_alias() == id)
            return &im;
    }

    std::string msg = "module `"+id+"` does not exist";
    throw InterpreterException(msg);
}

void
WorldCtx::define_class(StmtClass *klass) {
    const std::string &id = klass->m_id->lexeme();
    m_defined_classes.insert({id, klass});
}

bool
WorldCtx::class_is_defined(const std::string &id) const {
    return m_defined_classes.find(id) != m_defined_classes.end();
}

StmtClass *
WorldCtx::class_get(const std::string &id) {
    auto stmt = m_defined_classes.find(id);
    if (stmt == m_defined_classes.end())
        return nullptr;
    return stmt->second;
}

CtxType
WorldCtx::type(void) const {
    return CtxType::World;
}

void
WorldCtx::push_scope(void) {
    m_scope.push();
}

void
WorldCtx::pop_scope(void) {
    m_scope.pop();
}

void
WorldCtx::variable_add(std::shared_ptr<earl::variable::Obj> var) {
    const std::string &id = var->id();
    m_scope.add(id, var);
}

bool
WorldCtx::variable_exists(const std::string &id) {
    return m_scope.contains(id);
}

std::shared_ptr<earl::variable::Obj>
WorldCtx::variable_get(const std::string &id) {
    auto var = m_scope.get(id);
    if ((var->attrs() & static_cast<uint32_t>(Attr::Experimental)) != 0) {
        std::cerr << "warning: variable `"+var->id()+"` is marked as `experimental`" << std::endl;;
        var->disable_experimental_flag();
    }
    return var;
}

void
WorldCtx::variable_remove(const std::string &id) {
    assert(this->variable_exists(id));
    m_scope.remove(id);
}

void
WorldCtx::function_add(std::shared_ptr<earl::function::Obj> func) {
    const std::string &id = func->id();
    m_funcs.add(id, func);
}

bool
WorldCtx::function_exists(const std::string &id) {
    return m_funcs.contains(id);
}

std::shared_ptr<earl::function::Obj>
WorldCtx::function_get(const std::string &id) {
    return m_funcs.get(id);
}

void
WorldCtx::debug_dump_defined_classes(void) const {
    std::cout << "debug dumping classes for module: " << this->get_mod() << std::endl;

    for (auto it = m_defined_classes.begin(); it != m_defined_classes.end(); ++it) {
        std::cout << "  " << it->first << std::endl;
    }
}

bool
WorldCtx::closure_exists(const std::string &id) {
    auto f = m_scope.get(id);
    if (!f)
        return false;
    return f->type() == earl::value::Type::Closure;
}

void
WorldCtx::debug_dump_variables(void) const {
    std::cout << "Debug Dumping Variables for mod: " << this->get_mod() << std::endl;
    this->m_scope.debug_dump();
}

void
WorldCtx::enum_add(std::shared_ptr<earl::value::Enum> _enum) {
    const std::string &id = _enum->id();
    m_enums.insert({id, std::move(_enum)});
}

bool
WorldCtx::enum_exists(const std::string &id) const {
    return m_enums.find(id) != m_enums.end();
}

std::shared_ptr<earl::value::Enum>
WorldCtx::enum_get(const std::string &id) {
    return m_enums.find(id)->second;
}

WorldCtx *
WorldCtx::get_world(void) {
    return this;
}

void
WorldCtx::strip_funs_and_classes(void) {
    m_funcs.clear();
    m_defined_classes.clear();
}

std::vector<std::string>
WorldCtx::get_available_function_names(void) {
    std::vector<std::shared_ptr<earl::function::Obj>> funcs = m_funcs.extract_tovec();
    std::vector<std::string> ids = {};
    for (auto &f : funcs)
        ids.push_back(f->id());
    return ids;
}

std::vector<std::string>
WorldCtx::get_available_variable_names(void) {
    std::vector<std::shared_ptr<earl::variable::Obj>> vars = m_scope.extract_tovec();
    std::vector<std::string> ids = {};
    for (auto &v : vars)
        ids.push_back(v->id());
    return ids;
}

std::string
WorldCtx::get_filepath(void) const {
    return m_filepath;
}
