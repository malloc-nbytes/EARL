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

Int::Int(int value) : m_value(value) {}

int Int::value(void) {
    return m_value;
}

void Int::fill(int value) {
    m_value = value;
}

Type Int::type(void) const {
    return Type::Int;
}

std::shared_ptr<Obj> Int::binop(Token *op, Obj *other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("Int::binop");
}

bool Int::boolean(void) {
    return this->value();
}

void Int::mutate(Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot mutate (fix this message)");
    }

    switch (other->type()) {
    case Type::Int: {
        this->fill(dynamic_cast<Int *>(other)->value());
    } break;
    default: {
        assert(false && "unreachable");
    }
    }
}

std::shared_ptr<Obj> Int::copy(void) {
    UNIMPLEMENTED("Int::copy");
}

bool Int::eq(Obj *other) {
    if (other->type() != Type::Int)
        return false;
    return this->value() == dynamic_cast<Int *>(other)->value();
}

std::string Int::to_cxxstring(void) {
    return std::to_string(m_value);
}
