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
#include "intrinsics.hpp"

Ctx::Ctx(std::unique_ptr<Lexer> lexer, std::unique_ptr<Program> program, CtxType ctx_type) :
    m_lexer(std::move(lexer)), m_program(std::move(program)), m_module("") {
    m_parent = nullptr;
    m_ctx_type |= static_cast<uint32_t>(ctx_type);
}

/*** Ctx methods ***/

size_t Ctx::children_len(void) const {
    return m_children.size();
}

std::vector<std::shared_ptr<Ctx>> &Ctx::get_all_children(void) {
    return m_children;
}

std::shared_ptr<Ctx> Ctx::new_instance(CtxType ctx_type) {
    auto ctx = std::make_shared<Ctx>(nullptr, nullptr, ctx_type);

    ctx->m_functions = m_functions.copy();
    ctx->set_module(m_module + "-new-instance");

    auto &children = this->get_all_children();

    for (auto it = children.begin(); it != children.end(); ++it) {
        ctx->push_child_context(*it);
    }

    return ctx;
}

size_t Ctx::stmts_len(void) const {
    return m_program->m_stmts.size();
}

Stmt *Ctx::get_stmt_at(size_t i) {
    return m_program->m_stmts.at(i).get();
}

void Ctx::set_module(const std::string &id) {
    m_module = id;
}

void Ctx::push_scope(void) {
    m_variables.push();
    m_functions.push();
    // m_classes.push();
}

void Ctx::pop_scope(void) {
    m_variables.pop();
    m_functions.pop();
    // m_classes.pop();
}

void Ctx::set_parent(std::shared_ptr<Ctx> parent) {
    m_parent = parent;
}

std::shared_ptr<Ctx> Ctx::get_parent(void) {
    return m_parent;
}

void Ctx::push_child_context(std::shared_ptr<Ctx> ctx) {
    m_children.push_back(std::move(ctx));
}

std::shared_ptr<Ctx> Ctx::get_child_ctx(const std::string &id) {
    for (auto &ctx : m_children) {
        if (ctx->get_module() == id)
            return ctx;
    }
    return nullptr;
}

std::string &Ctx::get_module(void) {
    return m_module;
}

/*** Functions ***/
size_t Ctx::funcs_len(void) const {
    return m_functions.size();
}

void Ctx::func_add(std::shared_ptr<earl::function::Obj> func) {
    const std::string &id = func->id();
    m_functions.add(id, std::move(func));
}

bool Ctx::func_exists(const std::string &id) const {
    return m_functions.contains(id);
}

std::shared_ptr<earl::function::Obj> Ctx::func_get(const std::string &id, bool crash_on_failure) {
    std::shared_ptr<earl::function::Obj> func = m_functions.get(id);
    if (!func && crash_on_failure) {
        ERR_WARGS(Err::Type::Undeclared, "function `%s` has not been defined", id.c_str());
    }
    return func;
}

/*** Variables ***/
bool Ctx::var_exists(const std::string &id) const {
    return m_variables.contains(id);
}

std::shared_ptr<earl::variable::Obj> Ctx::var_get(const std::string &id, bool crash_on_failure) {
    std::shared_ptr<earl::variable::Obj> var = m_variables.get(id);
    if (!var && crash_on_failure) {
        ERR_WARGS(Err::Type::Undeclared, "variable `%s` is not in scope", id.c_str());
    }
    return var;
}

void Ctx::var_add(std::shared_ptr<earl::variable::Obj> var) {
    const std::string &id = var->id();
    m_variables.add(id, std::move(var));
}

void Ctx::var_remove(const std::string &id) {
    m_variables.remove(id);
}

void Ctx::var_debug_dump(void) {
    const std::string &mod = this->get_module();
    std::cout << "mod " << mod << " vars:" << std::endl;
    m_variables.debug_dump();
}

/*** Classes ***/
// bool Ctx::class_exists(const std::string &id) const {
//     UNIMPLEMENTED("Ctx::class_exists");
// }

// std::shared_ptr<earl::Class::Obj> &Ctx::class_get(const std::string &id, bool crash_on_failure) {
//     UNIMPLEMENTED("Ctx::class_get");
// }

// void Ctx::class_add(std::shared_ptr<earl::Class::Obj> klass) {
//     UNIMPLEMENTED("Ctx::class_add");
// }

void Ctx::class_make_available(StmtClass *klass) {
    const std::string &id = klass->m_id->lexeme();
    m_defined_classes.insert({id, klass});
}
