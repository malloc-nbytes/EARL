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
#include "common.hpp"

using namespace earl::value;

Option::Option(std::shared_ptr<Obj> value) : m_value(value) {}

std::shared_ptr<Obj> &
Option::value(void) {
    return m_value;
}

bool
Option::is_some(void) const {
    return m_value != nullptr;
}

bool
Option::is_none(void) const {
    return m_value == nullptr;
}

void
Option::set_value(std::shared_ptr<Obj> other) {
    (void)other;
    UNIMPLEMENTED("Option::set_value");
}

/*** OVERRIDES ***/
Type
Option::type(void) const {
    return Type::Option;
}

std::shared_ptr<Obj>
Option::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    auto other2 = dynamic_cast<Option *>(other);
    if (op->type() == TokenType::Double_Equals) {
        if (this->is_none() && other2->is_none())
            return std::make_shared<Bool>(true);

        if (this->is_some() && other2->is_none())
            return std::make_shared<Bool>(false);

        if (this->is_none() && other2->is_some())
            return std::make_shared<Bool>(false);

        return std::make_shared<Bool>(this->value()->eq(other2->value().get()));
    }
    else if (op->type() == TokenType::Bang_Equals) {
        if (this->is_none() && other2->is_none())
            return std::make_shared<Bool>(false);

        if (this->is_some() && other2->is_none())
            return std::make_shared<Bool>(true);

        if (this->is_none() && other2->is_some())
            return std::make_shared<Bool>(true);

        return std::make_shared<Bool>(!this->value()->eq(other2->value().get()));
    }
    else {
        Err::err_wtok(op);
        std::string msg = "invalid operator for binary operation `"+op->lexeme()+"` on option type";
        throw InterpreterException(msg);
    }
}

bool
Option::boolean(void) {
    return this->is_some();
}

void
Option::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    auto *other2 = dynamic_cast<Option *>(other);

    if (other2->is_none())
        m_value = nullptr;
    else
        m_value = other2->value();
}

std::shared_ptr<Obj>
Option::copy(void) {
    std::shared_ptr<Option> value = nullptr;
    if (m_value)
        value = std::make_shared<Option>(m_value->copy());
    else
        value = std::make_shared<Option>();
    if (m_info.has_value())
        value->set_info(m_info.value());
    return value;
}

bool
Option::eq(Obj *other) {
    if (other->type() != Type::Option)
        return false;
    auto other_option = dynamic_cast<Option *>(other);
    if (this->is_none() && other_option->is_some())
        return false;
    if (this->is_some() && other_option->is_none())
        return false;
    if (this->is_none() && other_option->is_none())
        return true;
    return m_value->eq(other);
}

std::shared_ptr<Obj>
Option::unaryop(Token *op) {
    switch (op->type()) {
    case TokenType::Bang: return std::make_shared<Bool>(this->is_none());
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid unary operator on option type";
        throw InterpreterException(msg);
    }
    }
    return nullptr; // unreachable
}

std::string
Option::to_cxxstring(void) {
    if (this->is_none())
        return COMMON_EARLKW_NONE;
    return "some(" + m_value->to_cxxstring() + ")";
}

std::shared_ptr<Obj>
Option::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    auto other2 = dynamic_cast<Option *>(other);

    if (op->type() == TokenType::Double_Equals) {
        if (this->is_none() && other2->is_none())
            return std::make_shared<Bool>(true);

        if (this->is_some() && other2->is_none())
            return std::make_shared<Bool>(false);

        if (this->is_none() && other2->is_some())
            return std::make_shared<Bool>(false);

        return std::make_shared<Bool>(this->value()->eq(other2->value().get()));
    }
    else if (op->type() == TokenType::Bang_Equals) {
        if (this->is_none() && other2->is_none())
            return std::make_shared<Bool>(false);

        if (this->is_some() && other2->is_none())
            return std::make_shared<Bool>(true);

        if (this->is_none() && other2->is_some())
            return std::make_shared<Bool>(true);

        return std::make_shared<Bool>(!this->value()->eq(other2->value().get()));
    }
    else {
        Err::err_wtok(op);
        std::string msg = "invalid operator for binary operation `"+op->lexeme()+"` on option type";
        throw InterpreterException(msg);
    }
    return nullptr; // unreachable
}
