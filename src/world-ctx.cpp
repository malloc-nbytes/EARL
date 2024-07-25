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

WorldCtx::WorldCtx(std::unique_ptr<Lexer> lexer, std::unique_ptr<Program> program)
    : m_lexer(std::move(lexer)), m_program(std::move(program)) {}

size_t WorldCtx::stmts_len(void) const {
    return m_program->m_stmts.size();
}

Stmt *WorldCtx::stmt_at(size_t idx) {
    return m_program->m_stmts.at(idx).get();
}

void WorldCtx::set_mod(std::string id) {
    m_mod = id;
}

CtxType WorldCtx::type(void) const {
    return CtxType::World;
}

void WorldCtx::push_variable_scope(void) {
    m_scope.push();
}

void WorldCtx::pop_variable_scope(void) {
    m_scope.pop();
}

void WorldCtx::variable_add(std::shared_ptr<earl::variable::Obj> var) {
    const std::string &id = var->id();
    m_scope.add(id, var);
}

bool WorldCtx::variable_exists(const std::string &id) {
    return m_scope.contains(id);
}

std::shared_ptr<earl::variable::Obj> WorldCtx::variable_get(const std::string &id) {
    return m_scope.get(id);
}

