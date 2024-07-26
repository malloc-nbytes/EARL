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

CtxType FunctionCtx::type(void) const {
    return CtxType::Function;
}

void FunctionCtx::push_variable_scope(void) {
    UNIMPLEMENTED("FunctionCtx::push_variable_scope");
}

void FunctionCtx::pop_variable_scope(void) {
    UNIMPLEMENTED("FunctionCtx::pop_variable_scope");
}

void FunctionCtx::variable_add(std::shared_ptr<earl::variable::Obj> var) {
    UNIMPLEMENTED("FunctionCtx::add_variable");
}

bool FunctionCtx::variable_exists(const std::string &id) {
    UNIMPLEMENTED("FunctionCtx::variable_exists");
}

std::shared_ptr<earl::variable::Obj> FunctionCtx::variable_get(const std::string &id) {
    return m_scope.get(id);
}

void FunctionCtx::function_add(std::shared_ptr<earl::function::Obj> func) {
    const std::string &id = func->id();
    m_funcs.add(id, func);
}

bool FunctionCtx::function_exists(const std::string &id) {
    return m_funcs.contains(id);
}

std::shared_ptr<earl::function::Obj> FunctionCtx::function_get(const std::string &id) {
    UNIMPLEMENTED("FunctionCtx::function_get");
}
