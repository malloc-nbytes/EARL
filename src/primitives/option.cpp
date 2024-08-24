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
Option::binop(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);

    auto other2 = dynamic_cast<Option *>(other.get());
    if (op->type() == TokenType::Double_Equals) {
        if (this->is_none() && other2->is_none())
            return std::make_shared<Bool>(true);

        if (this->is_some() && other2->is_none())
            return std::make_shared<Bool>(false);

        if (this->is_none() && other2->is_some())
            return std::make_shared<Bool>(false);

        return std::make_shared<Bool>(this->value()->eq(other2->value()));
    }
    else if (op->type() == TokenType::Bang_Equals) {
        if (this->is_none() && other2->is_none())
            return std::make_shared<Bool>(false);

        if (this->is_some() && other2->is_none())
            return std::make_shared<Bool>(true);

        if (this->is_none() && other2->is_some())
            return std::make_shared<Bool>(true);

        return std::make_shared<Bool>(!this->value()->eq(other2->value()));
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
Option::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other.get(), stmt);

    auto *other2 = dynamic_cast<Option *>(other.get());

    if (other2->is_none())
        m_value = nullptr;
    else
        m_value = other2->value();
}

std::shared_ptr<Obj>
Option::copy(void) {
    return std::make_shared<Option>(m_value);
}

bool
Option::eq(std::shared_ptr<Obj> &other) {
    if (other->type() != Type::Option)
        return false;
    auto other_option = dynamic_cast<Option *>(other.get());
    if (this->is_none() && other_option->is_some())
        return false;
    if (this->is_some() && other_option->is_none())
        return false;
    if (this->is_none() && other_option->is_none())
        return true;
    return m_value->eq(other);
}

std::string
Option::to_cxxstring(void) {
    if (this->is_none())
        return COMMON_EARLKW_NONE;
    return "some(" + m_value->to_cxxstring() + ")";
}

void
Option::spec_mutate(Token *op, const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    (void)other;
    Err::err_wtok(op);
    std::string msg = "invalid operator for special mutation `"+op->lexeme()+"` on option type";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Option::unaryop(Token *op) {
    if (op->type() == TokenType::Bang) {
        return std::make_shared<Bool>(this->is_none());
    }
    Err::err_wtok(op);
    std::string msg = "invalid unary operator on option type";
    throw InterpreterException(msg);
    return nullptr; // unreachable
}

