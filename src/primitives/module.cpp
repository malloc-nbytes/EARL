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
#include "utils.hpp"
#include "err.hpp"

using namespace earl::value;

Module::Module(std::shared_ptr<Ctx> ctx) : m_value(ctx) {}

std::shared_ptr<Ctx> &
Module::value(void) {
    UNIMPLEMENTED("Module::value");
}

Type
Module::type(void) const {
    return Type::Module;
}

std::shared_ptr<Obj>
Module::binop(Token *op, std::shared_ptr<Obj> &other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("Module::binop");
}

bool
Module::boolean(void) {
    UNIMPLEMENTED("Module::boolean");
}

void
Module::mutate(const std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("Module::mutate");
}

std::shared_ptr<Obj>
Module::copy(void) {
    UNIMPLEMENTED("Module::copy");
}

bool
Module::eq(std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("Module::eq");
}

std::string
Module::to_cxxstring(void) {
    ERR(Err::Type::Fatal, "unable to convert `module` type to a string");
}

void
Module::spec_mutate(Token *op, const std::shared_ptr<Obj> &other) {
    (void)op;
    (void)other;
    assert(false && "unreachable");
}

std::shared_ptr<Obj>
Module::unaryop(Token *op) {
    (void)op;
    Err::err_wtok(op);
    ERR(Err::Type::Fatal, "invalid unary operator on module type");
    return nullptr; // unreachable
}
