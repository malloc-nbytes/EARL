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

Ctx::Ctx(std::unique_ptr<Lexer> lexer, std::unique_ptr<Program> program) :
    m_curfunc(nullptr), m_module(nullptr), m_lexer(std::move(lexer)), m_program(std::move(program)) {}

void Ctx::set_function(earl::function::Obj *func) {
    m_stacktrace.push_back(func);
    m_curfunc = func;
}

Stmt *Ctx::get_stmt(size_t i) {
    return m_program->m_stmts[i].get();
}

size_t Ctx::stmts_len(void) const {
    return m_program->m_stmts.size();
}

void Ctx::unset_function(void) {
    assert(m_stacktrace.size() >= 1);
    m_stacktrace.pop_back();

    m_curfunc = m_stacktrace.empty()
        ? nullptr
        : m_stacktrace.back();
}

void Ctx::push_scope(void) {
    if (in_function()) {
        m_curfunc->push_scope();
    }
    else {
        m_globalvars.push();
        m_globalfuncs.push();
    }
}

void Ctx::pop_scope(void) {
    if (in_function()) {
        m_curfunc->pop_scope();
    }
    else {
        m_globalvars.pop();
        m_globalfuncs.pop();
    }
}

earl::value::Module *Ctx::get_registered_module(const std::string &id) {
    for (size_t i = 0; i < m_children_contexts.size(); ++i) {
        Ctx *child = m_children_contexts[i].get();

        if (child->get_module() && child->get_module()->lexeme() == id) {
            return new earl::value::Module(child);
        }
    }
    return nullptr;
}

earl::variable::Obj *Ctx::get_registered_variable(const std::string &id) {
    earl::variable::Obj **var = nullptr;

    if (in_function() && get_curfunc()->is_world()) {
        var = m_globalvars.get(id); // Check in global scope
        if (!var) { // Not in global, check local
            var = get_curfunc()->m_local.back().get(id);
        }
        else if (get_curfunc()->has_local(id)) { // Is in global, make sure its not in local
            ERR_WARGS(Err::Type::Redeclaration, "duplicate variable `%s`", id.c_str());
        }
    }
    else if (in_function()) {
        auto *func = get_curfunc();
        var = func->m_local.back().get(id);
    }
    else {
        var = m_globalvars.get(id);
    }

    if (!var) {
        ERR_WARGS(Err::Type::Fatal, "variable `%s` is not in scope", id.c_str());
    }

    return *var;
}

void Ctx::register_variable(earl::variable::Obj *var) {
    const std::string &id = var->id();

    if (in_function()) {
        get_curfunc()->m_local.back().add(id, var);
    }
    else {
        m_globalvars.add(id, var);
    }
}

bool Ctx::variable_is_registered(const std::string &id) {
    if (in_function()) {
        return get_curfunc()->has_local(id);
    }
    return m_globalvars.contains(id);
}

void Ctx::unregister_variable(const std::string &id) {
    if (in_function()) {
        get_curfunc()->m_local.back().remove(id);
    }
    else {
        m_globalvars.remove(id);
    }

}

bool Ctx::function_is_registered(const std::string &id) {
    return m_globalfuncs.contains(id);
}

void Ctx::register_function(earl::function::Obj *func) {
    const std::string &id = func->id();
    m_globalfuncs.add(id, func);
}

earl::function::Obj *Ctx::get_registered_function(const std::string &id) {
    earl::function::Obj **func = nullptr;
    func = m_globalfuncs.get(id);
    if (!func) {
        ERR_WARGS(Err::Type::Fatal,
                  "function `%s` is not in global scope",
                  id.c_str());
    }
    return *func;
}

earl::function::Obj *Ctx::get_curfunc(void) {
    assert(m_curfunc);
    return m_curfunc;
}

bool Ctx::in_function(void) const {
    return m_curfunc != nullptr;
}

Token *Ctx::get_module(void) {
    return m_module.get();
}

void Ctx::set_module(std::unique_ptr<Token> id) {
    if (m_module != nullptr) {
        ERR(Err::Type::Fatal, "Files are only limited to one module");
    }
    m_module = std::move(id);
}

void Ctx::push_child_context(std::unique_ptr<Ctx> child) {
    m_children_contexts.push_back(std::move(child));
}
