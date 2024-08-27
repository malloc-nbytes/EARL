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
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Type
Break::type(void) const {
    return Type::Break;
}

std::shared_ptr<Obj>
Break::binop(Token *op, std::shared_ptr<Obj> &other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("Break::binop");
}

bool
Break::boolean(void) {
    UNIMPLEMENTED("Break::boolean");
}

void
Break::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    (void)other;
    UNIMPLEMENTED("Break::mutate");
}

std::shared_ptr<Obj>
Break::copy(void) {
    UNIMPLEMENTED("Break::copy");
}

bool
Break::eq(std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("Break::eq");
}

std::string
Break::to_cxxstring(void) {
    UNIMPLEMENTED("Break::to_cxxstring");
}

void
Break::spec_mutate(Token *op, const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    (void)op;
    (void)other;
    assert(false && "unreachable");
}

std::shared_ptr<Obj>
Break::unaryop(Token *op) {
    (void)op;
    assert(false && "unreachable");
}

void
Break::set_const(void) {
    m_const = true;
}


