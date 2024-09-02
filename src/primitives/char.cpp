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
Char::binop(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);

    switch (op->type()) {
    case TokenType::Double_Equals: {
        return std::make_shared<Bool>(this->value() == dynamic_cast<Char *>(other.get())->value());
    } break;
    case TokenType::Bang_Equals: {
        return std::make_shared<Bool>(this->value() != dynamic_cast<Char *>(other.get())->value());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    } break;
    }
}

bool
Char::boolean(void) {
    UNIMPLEMENTED("Char::boolean");
}

void
Char::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other.get(), stmt);
    ASSERT_CONSTNESS(this, stmt);
    auto c = dynamic_cast<Char *>(other.get());
    m_value = c->value();
}

std::shared_ptr<Obj>
Char::copy(void) {
    return std::make_shared<Char>(m_value);
}

bool
Char::eq(std::shared_ptr<Obj> &other) {
    if (other->type() != Type::Char)
        return false;
    return this->value() == dynamic_cast<Char *>(other.get())->value();
}

std::string
Char::to_cxxstring(void) {
    return std::string(1, m_value);
}

void
Char::spec_mutate(Token *op, const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    (void)other;
    Err::err_wtok(op);
    std::string msg = "invalid operator for special mutation `"+op->lexeme()+"` on char type";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Char::unaryop(Token *op) {
    (void)op;
    Err::err_wtok(op);
    std::string msg = "invalid unary operator on char type";
    throw InterpreterException(msg);
    return nullptr; // unreachable
}

void
Char::set_const(void) {
    m_const = true;
}

