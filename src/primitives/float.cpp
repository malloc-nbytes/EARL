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
#include <cmath>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Float::Float(double value) : m_value(value) {}

double
Float::value(void) {
    return m_value;
}

void
Float::fill(double value) {
    m_value = value;
}

Type
Float::type(void) const {
    return Type::Float;
}

std::shared_ptr<Obj>
Float::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    switch (op->type()) {
    case TokenType::Plus: {
        return other->type() == Type::Int ?
            std::make_shared<Float>(this->value() + dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Float>(this->value() + dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Minus: {
        return other->type() == Type::Int ?
            std::make_shared<Float>(this->value() - dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Float>(this->value() - dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Asterisk: {
        return other->type() == Type::Int ?
            std::make_shared<Float>(this->value() * dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Float>(this->value() * dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Forwardslash: {
        return other->type() == Type::Int ?
            std::make_shared<Float>(this->value() / dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Float>(this->value() / dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Percent: {
        std::string msg = "cannot use module `%%` with floating point";
        throw InterpreterException(msg);
    } break;
    case TokenType::Double_Asterisk: {
        float p = 0.;
        if (other->type() == earl::value::Type::Float) {
            auto _other = dynamic_cast<earl::value::Float *>(other);
            p = std::pow(static_cast<float>(m_value), static_cast<float>(_other->value()));
        }
        else {
            auto _other = dynamic_cast<earl::value::Int *>(other);
            p = std::pow(static_cast<float>(m_value), static_cast<float>(_other->value()));
        }
        return std::make_shared<earl::value::Float>(static_cast<double>(p));
    } break;
    case TokenType::Lessthan: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() < dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() < dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Greaterthan: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() > dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() > dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Double_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() == dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() == dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Greaterthan_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() >= dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() >= dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Lessthan_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() <= dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() <= dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Bang_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() != dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() != dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Double_Pipe: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() || dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() || dynamic_cast<Float *>(other)->value());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    }
    }
}

bool
Float::boolean(void) {
    return this->value();
}

void
Float::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    switch (other->type()) {
    case Type::Float: {
        m_value = dynamic_cast<Float *>(other)->value();
    } break;
    case Type::Int: {
        m_value = static_cast<double>(dynamic_cast<Int *>(other)->value());
    } break;
    default: {
        assert(false && "unreachable");
    }
    }
}

std::shared_ptr<Obj>
Float::copy(void) {
    return std::make_shared<Float>(m_value);
}

bool
Float::eq(Obj *other) {
    if (other->type() == Type::Void)
        return true;

    if (other->type() != Type::Float)
        return false;
    return this->value() == dynamic_cast<Float *>(other)->value();
}

std::string
Float::to_cxxstring(void) {
    return std::to_string(m_value);
}

void
Float::spec_mutate(Token *op, Obj *other, StmtMut *stmt) {
    ASSERT_CONSTNESS(this, stmt);
    double prev;

    if (op->type() == TokenType::Minus_Equals
        || op->type() == TokenType::Forwardslash_Equals
        || op->type() == TokenType::Percent_Equals) {
        if (other->type() == Type::Int)
            prev = static_cast<int>(dynamic_cast<Int *>(other)->value());
        else if (other->type() == Type::Float)
            prev = dynamic_cast<Float *>(other)->value();
        else {
            const std::string msg = "invalid type for spec_mutate";
            Err::err_wtok(op);
            throw InterpreterException(msg);
        }
    }
    else {
        prev = m_value;
        this->mutate(other, stmt); // does type checking
    }

    this->mutate(other, stmt); // does type checking
    switch (op->type()) {
    case TokenType::Plus_Equals: m_value += prev; break;
    case TokenType::Minus_Equals: m_value -= prev; break;
    case TokenType::Asterisk_Equals: m_value *= prev; break;
    case TokenType::Forwardslash_Equals: m_value /= prev; break;
    case TokenType::Percent_Equals: {
        Err::err_wtok(op);
        std::string msg = "cannot use module `"+op->lexeme()+"` on float type";
        throw InterpreterException(msg);
    }
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid operator for special mutation `"+op->lexeme()+"`";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<Obj>
Float::unaryop(Token *op) {
    switch (op->type()) {
    case TokenType::Minus: return std::make_shared<Float>(-m_value);
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid unary operator on float type";
        throw InterpreterException(msg);
    }
    }
    return nullptr; // unreachable
}

void
Float::set_const(void) {
    m_const = true;
}

std::shared_ptr<Obj>
Float::add(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    return other->type() == Type::Int ?
        std::make_shared<Float>(this->value() + dynamic_cast<Int *>(other)->value()) :
        std::make_shared<Float>(this->value() + dynamic_cast<Float *>(other)->value());
}

std::shared_ptr<Obj>
Float::sub(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    return other->type() == Type::Int ?
        std::make_shared<Float>(this->value() - dynamic_cast<Int *>(other)->value()) :
        std::make_shared<Float>(this->value() - dynamic_cast<Float *>(other)->value());
}

std::shared_ptr<Obj>
Float::multiply(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    return other->type() == Type::Int ?
        std::make_shared<Float>(this->value() * dynamic_cast<Int *>(other)->value()) :
        std::make_shared<Float>(this->value() * dynamic_cast<Float *>(other)->value());
}

std::shared_ptr<Obj>
Float::divide(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    return other->type() == Type::Int ?
        std::make_shared<Float>(this->value() / dynamic_cast<Int *>(other)->value()) :
        std::make_shared<Float>(this->value() / dynamic_cast<Float *>(other)->value());
}

std::shared_ptr<Obj>
Float::power(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    float p = 0.f;
    if (other->type() == earl::value::Type::Float) {
        auto _other = dynamic_cast<earl::value::Float *>(other);
        p = std::pow(static_cast<float>(m_value), static_cast<float>(_other->value()));
    }
    else {
        auto _other = dynamic_cast<earl::value::Int *>(other);
        p = std::pow(static_cast<float>(m_value), static_cast<float>(_other->value()));
    }
    return std::make_shared<earl::value::Float>(static_cast<double>(p));
}

std::shared_ptr<Obj>
Float::gtequality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    switch (op->type()) {
    case TokenType::Lessthan: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() < dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() < dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Greaterthan: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() > dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() > dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Greaterthan_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() >= dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() >= dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Lessthan_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() <= dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() <= dynamic_cast<Float *>(other)->value());
    } break;
    default: {
    } break;
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    }
    return nullptr; // unreachable
}

std::shared_ptr<Obj>
Float::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    switch (op->type()) {
    case TokenType::Double_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() == dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() == dynamic_cast<Float *>(other)->value());
    } break;
    case TokenType::Bang_Equals: {
        return other->type() == Type::Int ?
            std::make_shared<Bool>(this->value() != dynamic_cast<Int *>(other)->value()) :
            std::make_shared<Bool>(this->value() != dynamic_cast<Float *>(other)->value());
    } break;
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
    return nullptr; // unreachable
}
