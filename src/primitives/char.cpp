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
#include <iostream>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Char::Char(char value) {
    m_value = value;
}

char
Char::value(void) {
    return m_value;
}

Type
Char::type(void) const {
    return Type::Char;
}

std::shared_ptr<Obj>
Char::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    switch (op->type()) {
    case TokenType::Double_Equals: {
        return std::make_shared<Bool>(this->value() == dynamic_cast<Char *>(other)->value());
    } break;
    case TokenType::Bang_Equals: {
        return std::make_shared<Bool>(this->value() != dynamic_cast<Char *>(other)->value());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    } break;
    }
}

void
Char::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);
    auto c = dynamic_cast<Char *>(other);
    m_value = c->value();
}

std::shared_ptr<Obj>
Char::copy(void) {
    auto value = std::make_shared<Char>(m_value);
    value->set_owner(m_var_owner);
    return value;
}

bool
Char::eq(Obj *other) {
    if (other->type() != Type::Char)
        return false;
    return this->value() == dynamic_cast<Char *>(other)->value();
}

std::string
Char::to_cxxstring(void) {
    return std::string(1, m_value);
}

std::shared_ptr<Obj>
Char::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    switch (op->type()) {
    case TokenType::Double_Equals: {
        if (other->type() == Type::Str) {
            auto str = dynamic_cast<Str *>(other);
            if (str->value().size() != 1)
                return std::make_shared<Bool>(false);
            return std::make_shared<Bool>(this->value() == str->value()[0]);
        }
        return std::make_shared<Bool>(this->value() == dynamic_cast<Char *>(other)->value());
    } break;
    case TokenType::Bang_Equals: {
        if (other->type() == Type::Str) {
            auto str = dynamic_cast<Str *>(other);
            if (str->value().size() != 1)
                return std::make_shared<Bool>(true);
            return std::make_shared<Bool>(this->value() != str->value()[0]);
        }
        return std::make_shared<Bool>(this->value() != dynamic_cast<Char *>(other)->value());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    } break;
    }
    return nullptr; // unreachable
}

std::shared_ptr<Obj>
Char::add(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    if (other->type() == Type::Char) {
        return std::make_shared<Str>(std::string(1, this->value())
                                     + std::string(1, dynamic_cast<Char *>(other)->value()));
    }
    return std::make_shared<Str>(std::string(1, this->value())
                                 + dynamic_cast<Str *>(other)->value());
}
