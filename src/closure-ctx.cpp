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
ClosureCtx::in_class(void) const {
    auto owner = m_owner.get();
    if (owner && owner->type() == CtxType::Class) {
        return true;
    }
    if (owner && owner->type() == CtxType::Function) {
        return dynamic_cast<FunctionCtx *>(owner)->in_class();
    }
    if (owner && owner->type() == CtxType::Closure) {
        return dynamic_cast<ClosureCtx *>(owner)->in_class();
    }
    return false;
}

// NOTE: in_class() should be called prior to using
//       this function.
std::shared_ptr<Ctx> &
ClosureCtx::get_outer_class_owner_ctx(void) {
    if (m_owner && m_owner->type() == CtxType::Class) {
        return m_owner;
    }
    if (m_owner && m_owner->type() == CtxType::Function) {
        return dynamic_cast<FunctionCtx *>(m_owner.get())->get_outer_class_owner_ctx();
    }
    assert(m_owner->type() == CtxType::Closure);
    return dynamic_cast<ClosureCtx *>(m_owner.get())->get_outer_class_owner_ctx();
}

bool
ClosureCtx::variable_exists(const std::string &id) {
    bool res = m_scope.contains(id);

    if (!res && m_owner && m_owner->type() == CtxType::Class)
        res = dynamic_cast<ClassCtx *>(m_owner.get())->variable_exists(id);

    if (!res && m_owner && m_owner->type() == CtxType::World)
        res = dynamic_cast<WorldCtx *>(m_owner.get())->variable_exists(id);

    if (!res && m_owner && m_owner->type() == CtxType::Function)
        res = dynamic_cast<FunctionCtx *>(m_owner.get())->variable_exists(id);

    if (!res && m_owner && m_owner->type() == CtxType::Closure)
        res = dynamic_cast<ClosureCtx *>(m_owner.get())->variable_exists(id);

    return res;
}

std::shared_ptr<earl::variable::Obj>
ClosureCtx::variable_get(const std::string &id) {
    std::shared_ptr<earl::variable::Obj> var = m_scope.get(id);

    if (!var && m_owner && m_owner->type() == CtxType::Class)
        var = dynamic_cast<ClassCtx *>(m_owner.get())->variable_get(id);

    if (!var && m_owner && m_owner->type() == CtxType::World)
        var = dynamic_cast<WorldCtx *>(m_owner.get())->variable_get(id);

    if (!var && m_owner && m_owner->type() == CtxType::Function)
        var = dynamic_cast<FunctionCtx *>(m_owner.get())->variable_get(id);

    if (!var && m_owner && m_owner->type() == CtxType::Closure)
        var = dynamic_cast<ClosureCtx *>(m_owner.get())->variable_get(id);

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
    auto f = m_scope.get(id);

    if ((!f || f->type() != earl::value::Type::Closure) && m_owner && m_owner->type() == CtxType::World) {
        bool maybe_world = dynamic_cast<WorldCtx *>(m_owner.get())->closure_exists(id);
        if (maybe_world)
            return true;
    }

    if ((!f || f->type() != earl::value::Type::Closure) && m_owner && m_owner->type() == CtxType::Function) {
        bool maybe_func = dynamic_cast<FunctionCtx *>(m_owner.get())->closure_exists(id);
        if (maybe_func)
            return true;
    }
    if ((!f || f->type() != earl::value::Type::Closure) && m_owner && m_owner->type() == CtxType::Closure) {
        bool maybe_cl = dynamic_cast<ClosureCtx *>(m_owner.get())->closure_exists(id);
        if (maybe_cl)
            return true;
    }

    if (!f)
        return false;
    return f->type() == earl::value::Type::Closure;
}

std::shared_ptr<Ctx> &
ClosureCtx::get_outer_world_owner(void) {
    if (m_owner && m_owner->type() == CtxType::Function)
        return dynamic_cast<FunctionCtx *>(m_owner.get())->get_outer_world_owner();
    else if (m_owner && m_owner->type() == CtxType::Class)
        return dynamic_cast<ClassCtx *>(m_owner.get())->get_owner();
    else if (m_owner && m_owner->type() == CtxType::Closure)
        return dynamic_cast<ClosureCtx *>(m_owner.get())->get_outer_world_owner();
    assert(m_owner);
    return m_owner;
}

void
ClosureCtx::assert_variable_does_not_exist_for_recursive_cl(const std::string &id) {
    if (m_scope.contains(id))
        goto bad;
    if (m_owner && m_owner->type() == CtxType::Function)
        if (dynamic_cast<FunctionCtx *>(m_owner.get())->variable_exists(id))
            goto bad;
    if (m_owner && m_owner->type() == CtxType::Class)
        assert(false && "unimplemented");
    return;

bad:
    std::string msg = "variable `"+id+"` is already declared";
    throw InterpreterException(msg);
}

WorldCtx *
ClosureCtx::get_world(void) {
    if (m_owner->type() == CtxType::Function)
        return dynamic_cast<FunctionCtx *>(m_owner.get())->get_world();
    else if (m_owner->type() == CtxType::Class)
        return dynamic_cast<ClassCtx *>(m_owner.get())->get_world();
    else if (m_owner->type() == CtxType::Closure)
        return dynamic_cast<ClosureCtx *>(m_owner.get())->get_world();
    return dynamic_cast<WorldCtx *>(m_owner.get());
}
