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

FunctionCtx::FunctionCtx(std::shared_ptr<Ctx> owner, uint32_t attrs)
    : m_immediate_owner(owner), m_attrs(attrs) {
    std::shared_ptr<Ctx> it = owner;
    while (1) {
        switch (it->type()) {
        case CtxType::World:
        case CtxType::Class: {
            m_owner = it;
            return;
        } break;
        case CtxType::Function: it = dynamic_cast<FunctionCtx *>(it.get())->m_owner; break;
        default: assert(false && "unreachable");
        }
    }
}

CtxType
FunctionCtx::type(void) const {
    return CtxType::Function;
}

void
FunctionCtx::push_scope(void) {
    m_scope.push();
    m_funcs.push();
}

void
FunctionCtx::pop_scope(void) {
    m_scope.pop();
    m_funcs.pop();
}

void
FunctionCtx::variable_add(std::shared_ptr<earl::variable::Obj> var) {
    const std::string &id = var->id();
    m_scope.add(id, var);
}

bool
FunctionCtx::variable_exists(const std::string &id) {
    bool res = m_scope.contains(id);

    bool world = (m_attrs & static_cast<uint32_t>(m_attrs)) != 0;

    if (!res && world && m_owner && m_owner->type() == CtxType::World)
        res = dynamic_cast<WorldCtx *>(m_owner.get())->variable_exists(id);

    if (!res && world && m_owner && m_owner->type() == CtxType::Function)
        res = dynamic_cast<FunctionCtx *>(m_owner.get())->variable_exists(id);

    if (!res && m_owner && m_owner->type() == CtxType::Class)
        res = dynamic_cast<ClassCtx *>(m_owner.get())->variable_exists(id);

    return res;
}

std::shared_ptr<earl::variable::Obj>
FunctionCtx::variable_get(const std::string &id) {
    std::shared_ptr<earl::variable::Obj> var = m_scope.get(id);

    bool world = (m_attrs & static_cast<uint32_t>(m_attrs)) != 0;

    if (!var && world && m_owner && m_owner->type() == CtxType::World)
        var = dynamic_cast<WorldCtx *>(m_owner.get())->variable_get(id);

    if (!var && world && m_owner && m_owner->type() == CtxType::Function)
        var = dynamic_cast<FunctionCtx *>(m_owner.get())->variable_get(id);

    if (!var && m_owner && m_owner->type() == CtxType::Class)
        var = dynamic_cast<ClassCtx *>(m_owner.get())->variable_get(id);

    return var;
}

void FunctionCtx::variable_remove(const std::string &id) {
    assert(this->variable_exists(id));
    m_scope.remove(id);
}

void
FunctionCtx::function_add(std::shared_ptr<earl::function::Obj> func) {
    const std::string &id = func->id();
    m_funcs.add(id, func);
}

// TODO: check for conflics
bool
FunctionCtx::function_exists(const std::string &id) {
    bool res = false;

    // Handle recurive nested functions.
    if (m_immediate_owner && m_immediate_owner->type() == CtxType::Function)
        res = dynamic_cast<FunctionCtx *>(m_immediate_owner.get())->function_exists(id);

    if (!res && m_owner && m_owner->type() == CtxType::Class)
        res = dynamic_cast<ClassCtx *>(m_owner.get())->function_exists(id);

    // May be in recursive chain, keep going backwards until
    // we hit the @world scope.
    if (!res && m_owner && m_owner->type() == CtxType::Function)
        res = dynamic_cast<FunctionCtx *>(m_owner.get())->function_exists(id);

    if (!res && m_owner->type() == CtxType::World)
        res = dynamic_cast<WorldCtx *>(m_owner.get())->function_exists(id);

    if (!res)
        res = m_funcs.contains(id);

    return res;
}

// TODO: check for conflics
std::shared_ptr<earl::function::Obj>
FunctionCtx::function_get(const std::string &id) {
    std::shared_ptr<earl::function::Obj> func = m_funcs.get(id);

    if (!func && m_immediate_owner && m_immediate_owner->type() == CtxType::Function)
        func = dynamic_cast<FunctionCtx *>(m_immediate_owner.get())->function_get(id);

    if (!func && m_owner && m_owner->type() == CtxType::Class)
        func = dynamic_cast<ClassCtx *>(m_owner.get())->function_get(id);

    if (!func && m_owner && m_owner->type() == CtxType::Function)
        func = dynamic_cast<FunctionCtx *>(m_owner.get())->function_get(id);

    if (!func && m_owner->type() == CtxType::World)
        func = dynamic_cast<WorldCtx *>(m_owner.get())->function_get(id);

    return func;
}

bool FunctionCtx::in_class(void) const {
    if (m_owner) {
        if (m_owner->type() == CtxType::Class)
            return true;
        else if (m_owner->type() == CtxType::Function)
            return dynamic_cast<FunctionCtx *>(m_owner.get())->in_class();
    }
    return false;
}

std::shared_ptr<Ctx> &FunctionCtx::get_outer_class_owner_ctx(void) {
    if (m_owner->type() == CtxType::Class)
        return m_owner;
    else if (m_owner->type() == CtxType::Function)
        return dynamic_cast<FunctionCtx *>(m_owner.get())->get_outer_class_owner_ctx();
    std::string msg = "Could not find outer class context for function context";
    throw InterpreterException(msg);
}

std::shared_ptr<Ctx> &FunctionCtx::get_owner(void) {
    return m_owner;
}

std::shared_ptr<Ctx> &FunctionCtx::get_outer_world_owner(void) {
    if (m_owner && m_owner->type() == CtxType::Function) {
        return dynamic_cast<FunctionCtx *>(m_owner.get())->get_outer_world_owner();
    }
    else if (m_owner && m_owner->type() == CtxType::Class) {
        return dynamic_cast<ClassCtx *>(m_owner.get())->get_owner();
    }

    assert(m_owner);
    return m_owner;
}

bool
FunctionCtx::closure_exists(const std::string &id) {
    auto f = m_scope.get(id);
    if (!f)
        return false;
    return f->type() == earl::value::Type::Closure;
}

void
FunctionCtx::debug_dump_variables(void) const {
    std::cout << "DEBUG DUMPING VARS" << std::endl;
    m_scope.debug_dump();
}

WorldCtx *
FunctionCtx::get_world(void) {
    if (m_owner->type() == CtxType::Class)
        return dynamic_cast<ClassCtx *>(m_owner.get())->get_world();
    else if (m_owner->type() == CtxType::Closure)
        return dynamic_cast<ClosureCtx *>(m_owner.get())->get_world();
    return dynamic_cast<WorldCtx *>(this->get_outer_world_owner().get());
}
