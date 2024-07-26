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

FunctionCtx::FunctionCtx(std::shared_ptr<Ctx> owner) {
    // TODO: check if function is @world, copy variables
    // from @world scope to this function's scope.
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
    return m_scope.contains(id);
}

std::shared_ptr<earl::variable::Obj>
FunctionCtx::variable_get(const std::string &id) {
    return m_scope.get(id);
}

void
FunctionCtx::function_add(std::shared_ptr<earl::function::Obj> func) {
    const std::string &id = func->id();
    m_funcs.add(id, func);
}

bool
FunctionCtx::function_exists(const std::string &id) {
    bool res = false;

    if (m_owner->type() == CtxType::World)
        res = dynamic_cast<WorldCtx *>(m_owner.get())->function_exists(id);
    if (!res)
        res = m_funcs.contains(id);

    return res;
}

std::shared_ptr<earl::function::Obj>
FunctionCtx::function_get(const std::string &id) {
    std::shared_ptr<earl::function::Obj> func = nullptr;

    if (m_owner->type() == CtxType::World)
        func = dynamic_cast<WorldCtx *>(m_owner.get())->function_get(id);
    if (!func)
        func = m_funcs.get(id);

    return func;
}
