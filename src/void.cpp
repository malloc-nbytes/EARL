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

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Void::Void(void *value) : m_value(value) {}

void *Void::value(void) {
    return m_value;
}

Type Void::type(void) const {
    return Type::Void;
}

Obj *Void::binop(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    ERR(Err::Type::Types, "invalid binary operation on Void types");
    return nullptr; // unreachable
}

bool Void::boolean(void) {
    return false;
}

void Void::mutate(Obj *other) {
    (void)other;
    UNIMPLEMENTED("Void::mutate");
}

Obj *Void::copy(void) {
    UNIMPLEMENTED("Void::copy");
}

bool Void::eq(Obj *other) {
    UNIMPLEMENTED("Void::eq");
}
