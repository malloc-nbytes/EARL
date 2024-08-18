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

ClassCtx::ClassCtx(std::shared_ptr<Ctx> owner) : m_owner(owner) {}

ClassCtx::ClassCtx(std::shared_ptr<Ctx> owner, SharedScope<std::string, earl::variable::Obj> scope)
    : m_owner(owner) {
    m_scope = std::move(scope);
}

CtxType
ClassCtx::type(void) const {
    return CtxType::Class;
}

void
ClassCtx::push_scope(void) {
    UNIMPLEMENTED("ClassCtx::push_variable_scope");
}

void
ClassCtx::pop_scope(void) {
    UNIMPLEMENTED("ClassCtx::pop_variable_scope");
}

std::vector<std::shared_ptr<earl::variable::Obj>>
ClassCtx::get_printable_members(void) {
    std::vector<std::shared_ptr<earl::variable::Obj>> members = m_scope.extract_tovec();
    return members;
}

void
ClassCtx::variable_add(std::shared_ptr<earl::variable::Obj> var) {
    const std::string &id = var->id();
    m_scope.add(id, var);
}

bool
ClassCtx::variable_exists(const std::string &id) {
    bool res = m_scope.contains(id);
    // ONLY USED FOR THE CONSTRUCTOR IF IT NEEDS IT!
    if (!res)
        res = __m_class_constructor_tmp_args.find(id)
            != __m_class_constructor_tmp_args.end();
    return res;
}

bool
ClassCtx::variable_exists_wo__m_class_constructor_tmp_args(const std::string &id) {
    return m_scope.contains(id);
}

std::shared_ptr<earl::variable::Obj>
ClassCtx::variable_get(const std::string &id) {
    auto var = m_scope.get(id);

    // ONLY USED FOR THE CONSTRUCTOR IF IT NEEDS IT!
    if (!var && __m_class_constructor_tmp_args.size() != 0) {
        auto it = __m_class_constructor_tmp_args.find(id);
        if (it != __m_class_constructor_tmp_args.end())
            var = it->second;
    }

    return var;
}

void ClassCtx::variable_remove(const std::string &id) {
    assert(this->variable_exists(id));
    m_scope.remove(id);
}

void
ClassCtx::function_add(std::shared_ptr<earl::function::Obj> func) {
    const std::string &id = func->id();
    m_funcs.add(id, func);
}

bool
ClassCtx::function_exists(const std::string &id) {
    bool res = false;
    if (m_owner && m_owner->type() == CtxType::World)
        res = dynamic_cast<WorldCtx *>(m_owner.get())->function_exists(id);
    if (!res)
        res = m_funcs.contains(id);
    return res;
}

std::shared_ptr<earl::function::Obj>
ClassCtx::function_get(const std::string &id) {
    std::shared_ptr<earl::function::Obj> func = nullptr;
    if (m_owner && m_owner->type() == CtxType::World)
        func = dynamic_cast<WorldCtx *>(m_owner.get())->function_get(id);
    if (!func)
        func = m_funcs.get(id);
    return func;
}

void
ClassCtx::function_debug_dump(void) const {
    m_funcs.debug_dump();
}

std::shared_ptr<Ctx> &
ClassCtx::get_owner(void) {
    assert(m_owner);
    return m_owner;
}

std::shared_ptr<Ctx> &
ClassCtx::get_world_owner(void) {
    if (m_owner && m_owner->type() == CtxType::World)
        return m_owner;
    if (m_owner && m_owner->type() == CtxType::Function)
        return dynamic_cast<FunctionCtx *>(m_owner.get())->get_outer_world_owner();
    assert(false && "unreachable");
}

bool
ClassCtx::closure_exists(const std::string &id) {
    auto f = m_scope.get(id);
    if (!f)
        return false;
    return f->type() == earl::value::Type::Closure;
}

void
ClassCtx::fill___m_class_constructor_tmp_args(std::shared_ptr<earl::variable::Obj> &var) {
    const std::string &id = var->id();
    __m_class_constructor_tmp_args.insert({var->id(), var});
}

void
ClassCtx::clear___m_class_constructor_tmp_args(void) {
    __m_class_constructor_tmp_args.clear();
}

std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> &
ClassCtx::get___m_class_constructor_tmp_args(void) {
    return __m_class_constructor_tmp_args;
}

std::shared_ptr<ClassCtx>
ClassCtx::deep_copy(void) {
    SharedScope<std::string, earl::variable::Obj> scope_copy;
    for (size_t i = 0; i < m_scope.m_map.size(); ++i) {
        auto el = m_scope.m_map.at(i);
        for (auto it = el.begin(); it != el.end(); ++it)
            scope_copy.add(it->first, it->second->copy());
        if (i != m_scope.size())
            scope_copy.push();
    }
    return std::make_shared<ClassCtx>(m_owner, std::move(scope_copy));
}

std::shared_ptr<ClassCtx>
ClassCtx::shallow_copy(void) {
    SharedScope<std::string, earl::variable::Obj> scope_copy;
    for (size_t i = 0; i < m_scope.m_map.size(); ++i) {
        auto el = m_scope.m_map.at(i);
        for (auto it = el.begin(); it != el.end(); ++it)
            scope_copy.add(it->first, it->second);
        if (i != m_scope.size())
            scope_copy.push();
    }
    return std::make_shared<ClassCtx>(m_owner, std::move(scope_copy));
}

WorldCtx *
ClassCtx::get_world(void) {
    if (m_owner && m_owner->type() == CtxType::Function)
        return dynamic_cast<WorldCtx *>(dynamic_cast<FunctionCtx *>(m_owner.get())->get_outer_world_owner().get());
    return dynamic_cast<WorldCtx *>(this->get_owner().get());
}
