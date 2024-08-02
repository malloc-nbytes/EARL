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

ClosureCtx::ClosureCtx(std::shared_ptr<Ctx> owner) : m_owner(owner) {}

CtxType
ClosureCtx::type(void) const {
    return CtxType::Closure;
}

void
ClosureCtx::push_scope(void) {
    m_scope.push();
    m_funcs.push();
}

void
ClosureCtx::pop_scope(void) {
    m_scope.pop();
    m_funcs.pop();
}

void
ClosureCtx::variable_add(std::shared_ptr<earl::variable::Obj> var) {
    const std::string &id = var->id();
    m_scope.add(id, var);
}

bool
ClosureCtx::variable_exists(const std::string &id) {
    bool res = false;

    if (m_owner && m_owner->type() == CtxType::Class)
        res = dynamic_cast<ClassCtx *>(m_owner.get())->variable_exists(id);

    if (m_owner && m_owner->type() == CtxType::World)
        res = dynamic_cast<WorldCtx *>(m_owner.get())->variable_exists(id);

    if (m_owner && m_owner->type() == CtxType::Function)
        res = dynamic_cast<FunctionCtx *>(m_owner.get())->variable_exists(id);

    if (!res)
        res = m_scope.contains(id);

    return res;
}

std::shared_ptr<earl::variable::Obj>
ClosureCtx::variable_get(const std::string &id) {
    std::shared_ptr<earl::variable::Obj> var = nullptr;

    if (m_owner && m_owner->type() == CtxType::Class)
        var = dynamic_cast<ClassCtx *>(m_owner.get())->variable_get(id);

    if (m_owner && m_owner->type() == CtxType::World)
        var = dynamic_cast<WorldCtx *>(m_owner.get())->variable_get(id);

    if (m_owner && m_owner->type() == CtxType::Function)
        var = dynamic_cast<FunctionCtx *>(m_owner.get())->variable_get(id);

    if (!var)
        var = m_scope.get(id);

    return var;
}

void
ClosureCtx::variable_remove(const std::string &id) {
    assert(this->variable_exists(id));
    m_scope.remove(id);
}

void
ClosureCtx::function_add(std::shared_ptr<earl::function::Obj> func) {
    const std::string &id = func->id();
    m_funcs.add(id, func);
}

bool
ClosureCtx::function_exists(const std::string &id) {
    bool res = false;

    if (m_owner && m_owner->type() == CtxType::Class)
        res = dynamic_cast<ClassCtx *>(m_owner.get())->function_exists(id);

    // May be in recursive chain, keep going backwards until
    // we hit the @world scope.
    if (m_owner && m_owner->type() == CtxType::Function)
        res = dynamic_cast<FunctionCtx *>(m_owner.get())->function_exists(id);

    if (m_owner->type() == CtxType::World)
        res = dynamic_cast<WorldCtx *>(m_owner.get())->function_exists(id);

    if (!res)
        res = m_funcs.contains(id);

    return res;
}

std::shared_ptr<earl::function::Obj>
ClosureCtx::function_get(const std::string &id) {
    std::shared_ptr<earl::function::Obj> func = nullptr;

    if (m_owner && m_owner->type() == CtxType::Class)
        func = dynamic_cast<ClassCtx *>(m_owner.get())->function_get(id);

    if (m_owner && m_owner->type() == CtxType::Function)
        func = dynamic_cast<FunctionCtx *>(m_owner.get())->function_get(id);

    if (m_owner->type() == CtxType::World)
        func = dynamic_cast<WorldCtx *>(m_owner.get())->function_get(id);

    if (!func)
        func = m_funcs.get(id);

    return func;
}

std::shared_ptr<Ctx> &
ClosureCtx::get_owner(void) {
    return m_owner;
}

bool
ClosureCtx::closure_exists(const std::string &id) {
    UNIMPLEMENTED("ClosureCtx::closure_exists");
}


