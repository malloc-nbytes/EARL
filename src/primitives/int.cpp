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

#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Int::Int(int value) : m_value(value) {}

int
Int::value(void) {
    return m_value;
}

void
Int::fill(int value) {
    m_value = value;
}

void
Int::incr(void) {
    ++m_value;
}

Type
Int::type(void) const {
    return Type::Int;
}

std::shared_ptr<Obj>
Int::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    switch (op->type()) {
    case TokenType::Plus: {
        if (other->type() == Type::Float)
            return std::make_shared<Float>(this->value() + dynamic_cast<Float *>(other)->value());
        else
            return std::make_shared<Int>(this->value() + dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Minus: {
        if (other->type() == Type::Float)
            return std::make_shared<Float>(this->value() - dynamic_cast<Float *>(other)->value());
        else
            return std::make_shared<Int>(this->value() - dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Asterisk: {
        if (other->type() == Type::Float)
            return std::make_shared<Float>(this->value() * dynamic_cast<Float *>(other)->value());
        else
            return std::make_shared<Int>(this->value() * dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Forwardslash: {
        if (other->type() == Type::Float)
            return std::make_shared<Float>(this->value() / dynamic_cast<Float *>(other)->value());
        else
            return std::make_shared<Int>(this->value() / dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Percent: {
        if (other->type() == Type::Float) {
            std::string msg = "cannot use module `%%` with floating point";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() % dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Double_Asterisk: {
        if (other->type() == earl::value::Type::Float) {
            auto _other = dynamic_cast<earl::value::Float *>(other);
            float p = std::pow(static_cast<float>(m_value), static_cast<float>(_other->value()));
            return std::make_shared<earl::value::Float>(p);
        }
        auto _other = dynamic_cast<earl::value::Int *>(other);
        int p = static_cast<int>(std::pow(static_cast<float>(m_value), static_cast<float>(_other->value())));
        return std::make_shared<earl::value::Int>(p);
    } break;
    case TokenType::Lessthan: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() < dynamic_cast<Float *>(other)->value()) :
            std::make_shared<Bool>(this->value() < dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Greaterthan: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() > dynamic_cast<Float *>(other)->value()) :
            std::make_shared<Bool>(this->value() > dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Double_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() == dynamic_cast<Float *>(other)->value()) :
            std::make_shared<Bool>(this->value() == dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Greaterthan_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() >= dynamic_cast<Float *>(other)->value()) :
            std::make_shared<Bool>(this->value() >= dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Lessthan_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() <= dynamic_cast<Float *>(other)->value()) :
            std::make_shared<Bool>(this->value() <= dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Bang_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() != dynamic_cast<Float *>(other)->value()) :
            std::make_shared<Bool>(this->value() != dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Double_Pipe: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() || dynamic_cast<Float *>(other)->value()) :
            std::make_shared<Bool>(this->value() || dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Double_Lessthan: {
        if (other->type() != Type::Int) {
            Err::err_wtok(op);
            const std::string msg = "cannot perform `"+op->lexeme()+"` with a float as the expression";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() << dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Double_Greaterthan: {
        if (other->type() != Type::Int) {
            Err::err_wtok(op);
            const std::string msg = "cannot perform `"+op->lexeme()+"` with a float as the expression";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() >> dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Backtick_Pipe: {
        if (other->type() != Type::Int) {
            Err::err_wtok(op);
            const std::string msg = "cannot perform `"+op->lexeme()+"` with a float as the expression";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() | dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Backtick_Caret: {
        if (other->type() != Type::Int) {
            Err::err_wtok(op);
            const std::string msg = "cannot perform `"+op->lexeme()+"` with a float as the expression";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() ^ dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Backtick_Ampersand: {
        if (other->type() != Type::Int) {
            Err::err_wtok(op);
            const std::string msg = "cannot perform `"+op->lexeme()+"` with a float as the expression";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() & dynamic_cast<Int *>(other)->value());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    }
    }
}

bool
Int::boolean(void) {
    return this->value();
}

void
Int::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    switch (other->type()) {
    case Type::Int: {
        m_value = dynamic_cast<Int *>(other)->value();
    } break;
    case Type::Float: {
        m_value = static_cast<int>(dynamic_cast<Float *>(other)->value());
    } break;
    default: {
        assert(false && "unreachable");
    }
    }
}

std::shared_ptr<Obj>
Int::copy(void) {
    return std::make_shared<Int>(m_value);
}

bool
Int::eq(Obj *other) {
    if (other->type() == Type::Void)
        return true;

    if (other->type() != Type::Int)
        return false;
    return this->value() == dynamic_cast<Int *>(other)->value();
}

std::string
Int::to_cxxstring(void) {
    return std::to_string(m_value);
}

void
Int::spec_mutate(Token *op, Obj *other, StmtMut *stmt) {
    ASSERT_CONSTNESS(this, stmt);
    int prev;

    if (op->type() == TokenType::Minus_Equals
        || op->type() == TokenType::Forwardslash_Equals
        || op->type() == TokenType::Percent_Equals) {
        if (other->type() == Type::Int)
            prev = dynamic_cast<Int *>(other)->value();
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

    switch (op->type()) {
    case TokenType::Plus_Equals: m_value += prev; break;
    case TokenType::Minus_Equals: m_value -= prev; break;
    case TokenType::Asterisk_Equals: m_value *= prev; break;
    case TokenType::Forwardslash_Equals: m_value /= prev; break;
    case TokenType::Percent_Equals: m_value %= prev; break;
    case TokenType::Backtick_Pipe_Equals: m_value |= prev; break;
    case TokenType::Backtick_Ampersand_Equals: m_value &= prev; break;
    case TokenType::Backtick_Caret_Equals: m_value ^= prev; break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid operator for special mutation `"+op->lexeme()+"`";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<Obj>
Int::unaryop(Token *op) {
    switch (op->type()) {
    case TokenType::Minus: return std::make_shared<Int>(-m_value);
    case TokenType::Bang: return std::make_shared<Bool>(!m_value);
    case TokenType::Backtick_Tilde: return std::make_shared<Int>(~m_value);
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid unary operator on int type";
        throw InterpreterException(msg);
    }
    }
    return nullptr; // unreachable
}

void
Int::set_const(void) {
    m_const = true;
}

std::shared_ptr<Obj>
Int::add(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);

    if (other->type() == Type::Float)
        return std::make_shared<Float>(this->value() + dynamic_cast<Float *>(other.get())->value());
    return std::make_shared<Int>(this->value() + dynamic_cast<Int *>(other.get())->value());
}

std::shared_ptr<Obj>
Int::sub(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);
    if (other->type() == Type::Float)
        return std::make_shared<Float>(this->value() - dynamic_cast<Float *>(other.get())->value());
    return std::make_shared<Int>(this->value() - dynamic_cast<Int *>(other.get())->value());
}

std::shared_ptr<Obj>
Int::multiply(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);
    if (other->type() == Type::Float)
        return std::make_shared<Float>(this->value() * dynamic_cast<Float *>(other.get())->value());
    return std::make_shared<Int>(this->value() * dynamic_cast<Int *>(other.get())->value());
}

std::shared_ptr<Obj>
Int::divide(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);
    if (other->type() == Type::Float)
        return std::make_shared<Float>(this->value() / dynamic_cast<Float *>(other.get())->value());
    return std::make_shared<Int>(this->value() / dynamic_cast<Int *>(other.get())->value());
}

std::shared_ptr<Obj>
Int::modulo(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_EXACT(this, other.get(), op);
    return std::make_shared<Int>(this->value() % dynamic_cast<Int *>(other.get())->value());
}

std::shared_ptr<Obj>
Int::power(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_EXACT(this, other.get(), op);
    auto _other = dynamic_cast<earl::value::Int *>(other.get());
    int p = static_cast<int>(std::pow(static_cast<float>(m_value), static_cast<float>(_other->value())));
    return std::make_shared<earl::value::Int>(p);
}

std::shared_ptr<Obj>
Int::gtequality(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);
    switch (op->type()) {
    case TokenType::Lessthan: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() < dynamic_cast<Float *>(other.get())->value()) :
            std::make_shared<Bool>(this->value() < dynamic_cast<Int *>(other.get())->value());
    } break;
    case TokenType::Greaterthan: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() > dynamic_cast<Float *>(other.get())->value()) :
            std::make_shared<Bool>(this->value() > dynamic_cast<Int *>(other.get())->value());
    } break;
    case TokenType::Greaterthan_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() >= dynamic_cast<Float *>(other.get())->value()) :
            std::make_shared<Bool>(this->value() >= dynamic_cast<Int *>(other.get())->value());
    } break;
    case TokenType::Lessthan_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() <= dynamic_cast<Float *>(other.get())->value()) :
            std::make_shared<Bool>(this->value() <= dynamic_cast<Int *>(other.get())->value());
    } break;
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<Obj>
Int::equality(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);
    switch (op->type()) {
    case TokenType::Double_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() == dynamic_cast<Float *>(other.get())->value()) :
            std::make_shared<Bool>(this->value() == dynamic_cast<Int *>(other.get())->value());
    } break;
    case TokenType::Bang_Equals: {
        return other->type() == Type::Float ?
            std::make_shared<Bool>(this->value() != dynamic_cast<Float *>(other.get())->value()) :
            std::make_shared<Bool>(this->value() != dynamic_cast<Int *>(other.get())->value());
    } break;
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<Obj>
Int::bitwise(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_EXACT(this, other.get(), op);
    switch (op->type()) {
    case TokenType::Backtick_Pipe:      return std::make_shared<Int>(this->value() | dynamic_cast<Int *>(other.get())->value());
    case TokenType::Backtick_Caret:     return std::make_shared<Int>(this->value() ^ dynamic_cast<Int *>(other.get())->value());
    case TokenType::Backtick_Ampersand: return std::make_shared<Int>(this->value() & dynamic_cast<Int *>(other.get())->value());
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<Obj>
Int::bitshift(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_EXACT(this, other.get(), op);
    switch (op->type()) {
    case TokenType::Double_Lessthan: {
        if (other->type() != Type::Int) {
            Err::err_wtok(op);
            const std::string msg = "cannot perform `"+op->lexeme()+"` with a float as the expression";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() << dynamic_cast<Int *>(other.get())->value());
    } break;
    case TokenType::Double_Greaterthan: {
        if (other->type() != Type::Int) {
            Err::err_wtok(op);
            const std::string msg = "cannot perform `"+op->lexeme()+"` with a float as the expression";
            throw InterpreterException(msg);
        }
        return std::make_shared<Int>(this->value() >> dynamic_cast<Int *>(other.get())->value());
    } break;
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
    return nullptr; // unreachable
}
