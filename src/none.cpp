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

None::None() : m_value(nullptr) {}

void None::set_value(Obj *other) {
    m_value = other;
}

Obj *None::value(void) {
    return m_value;
}

bool None::empty(void) {
    return this->value() == nullptr;
}

Type None::type(void) const {
    return Type::None;
}

Obj *None::binop(Token *op, Obj *other) {
    if (!this->value()) {
        ERR(Err::Type::Fatal, "can not perform binary operations on None type");
    }

    switch (this->value()->type()) {
    case earl::value::Type::Int: {
        if (other->type() == Type::None) {
            auto *other2 = dynamic_cast<None *>(other);
            return dynamic_cast<Int *>(this->value())->binop(op, other2->value());
        }
        return dynamic_cast<Int *>(this->value())->binop(op, other);
    } break;
    case earl::value::Type::Char: {
        if (other->type() == Type::None) {
            auto *other2 = dynamic_cast<None *>(other);
            return dynamic_cast<Char *>(this->value())->binop(op, other2->value());
        }
        return dynamic_cast<Char *>(this->value())->binop(op, other);
    } break;
    case earl::value::Type::Bool: {
        if (other->type() == Type::None) {
            auto *other2 = dynamic_cast<None *>(other);
            return dynamic_cast<Bool *>(this->value())->binop(op, other2->value());
        }
        return dynamic_cast<Bool *>(this->value())->binop(op, other);
    } break;
    case earl::value::Type::Str: {
        if (other->type() == Type::None) {
            auto *other2 = dynamic_cast<None *>(other);
            return dynamic_cast<Str *>(this->value())->binop(op, other2->value());
        }
        return dynamic_cast<Str *>(this->value())->binop(op, other);
    } break;
    case earl::value::Type::List: {
        if (other->type() == Type::None) {
            auto *other2 = dynamic_cast<None *>(other);
            return dynamic_cast<List *>(this->value())->binop(op, other2->value());
        }
        return dynamic_cast<List *>(this->value())->binop(op, other);
    } break;
    default: {
        ERR_WARGS(Err::Type::Fatal, "can not perform binary operation on type `%d`", static_cast<int>(this->value()->type()));
    }
    }

    return nullptr; // unreachable
}

bool None::boolean(void) {
    ERR(Err::Type::Fatal, "can not get Bool type on None type");
}

void None::mutate(Obj *other) {
    this->set_value(other);
}

Obj *None::copy(void) {
    auto *none = new None();
    none->set_value(this->value()->copy());
    return none;
}

bool None::eq(Obj *other) {
    UNIMPLEMENTED("None::eq");
}
