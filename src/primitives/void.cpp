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

#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Void::Void(void *value) : m_value(value) {}

Type
Void::type(void) const {
    return Type::Void;
}

std::shared_ptr<Obj>
Void::binop(Token *op, std::shared_ptr<Obj> &other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("Void::binop");
}

bool
Void::boolean(void) {
    return true;
}

void
Void::mutate(const std::shared_ptr<Obj> &other) {
    ASSERT_MUTATE_COMPAT(this, other.get());
}

std::shared_ptr<Obj>
Void::copy(void) {
    return std::make_shared<Void>();
}

bool
Void::eq(std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("Void::eq");
}

std::string
Void::to_cxxstring(void) {
    return "<unit>";
}

void
Void::spec_mutate(Token *op, const std::shared_ptr<Obj> &other) {
    (void)other;
    Err::err_wtok(op);
    ERR_WARGS(Err::Type::Fatal, "invalid operator for special mutation `%s` on unit type", op->lexeme().c_str());
}

std::shared_ptr<Obj>
Void::unaryop(Token *op) {
    (void)op;
    Err::err_wtok(op);
    ERR(Err::Type::Fatal, "invalid unary operator on unit type");
    return nullptr; // unreachable
}
