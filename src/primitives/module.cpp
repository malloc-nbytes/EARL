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

#include "earl.hpp"
#include "utils.hpp"

using namespace earl::value;

Module::Module(Ctx *ctx) : m_value(ctx) {}

Ctx *Module::value(void) {
    assert(m_value);
    return m_value;
}

Type Module::type(void) const {
    return Type::Module;
}

Obj *Module::binop(Token *op, Obj *other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("Module::binop");
}

bool Module::boolean(void) {
    UNIMPLEMENTED("Module::boolean");
}

void Module::mutate(Obj *other) {
    (void)other;
    UNIMPLEMENTED("Module::mutate");
}

Obj *Module::copy(void) {
    UNIMPLEMENTED("Module::copy");
}

bool Module::eq(Obj *other) {
    (void)other;
    UNIMPLEMENTED("Module::eq");
}
