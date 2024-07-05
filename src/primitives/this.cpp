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
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

This::This() {}

Type This::type(void) const {
    return Type::This;
}

Obj *This::binop(Token *op, Obj *other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("This::binop");
}

bool This::boolean(void) {
    UNIMPLEMENTED("This::boolean");
}

void This::mutate(Obj *other) {
    (void)other;
    UNIMPLEMENTED("This::mutate");
}

Obj *This::copy(void) {
    UNIMPLEMENTED("This::copy");
}

bool This::eq(Obj *other) {
    (void)other;
    UNIMPLEMENTED("This::eq");
}

std::string This::to_cxxstring(void) {
    ERR(Err::Type::Fatal, "unable to convert `this` type to a string");
}