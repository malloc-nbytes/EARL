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
#include <ctime>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Time::Time(void) : m_now(std::time(nullptr)) {}

Time::Time(std::time_t now) : m_now(now) {}

std::shared_ptr<Tuple>
Time::readable(void) {
    std::tm *local = std::localtime(&m_now);
    std::vector<std::shared_ptr<Obj>> values = {
        this->years(),
        this->months(),
        this->days(),
        this->hours(),
        this->minutes(),
        this->seconds(),
    };
    return std::make_shared<Tuple>(std::move(values));
}

std::shared_ptr<Int>
Time::raw(void) {
    int r = static_cast<int>(m_now);
    return std::make_shared<Int>(r);
}

std::shared_ptr<Time>
Time::update(void) {
    return std::make_shared<Time>(m_now);
}

std::shared_ptr<Int>
Time::years(void) {
    return std::make_shared<Int>(std::localtime(&m_now)->tm_year+1900);
}

std::shared_ptr<Int>
Time::months(void) {
    return std::make_shared<Int>(std::localtime(&m_now)->tm_mon+1);
}

std::shared_ptr<Int>
Time::days(void) {
    return std::make_shared<Int>(std::localtime(&m_now)->tm_mday);
}

std::shared_ptr<Int>
Time::hours(void) {
    return std::make_shared<Int>(std::localtime(&m_now)->tm_hour);
}

std::shared_ptr<Int>
Time::minutes(void) {
    return std::make_shared<Int>(std::localtime(&m_now)->tm_min);
}

std::shared_ptr<Int>
Time::seconds(void) {
    return std::make_shared<Int>(std::localtime(&m_now)->tm_sec);
}

// Implements
Type
Time::type(void) const {
    return Type::Time;
}

void Time::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);
    m_now = dynamic_cast<Time *>(other)->m_now;
}

std::shared_ptr<Obj>
Time::copy(void) {
    return std::make_shared<Time>(m_now);
}

bool
Time::eq(Obj *other) {
    if (other->type() != Type::Time)
        return false;
    return m_now == dynamic_cast<Time *>(other)->m_now;
}

std::string
Time::to_cxxstring(void) {
    return std::to_string(m_now);
}

std::shared_ptr<Obj>
Time::gtequality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    auto time2 = dynamic_cast<Time *>(other);
    switch (op->type()) {
    case TokenType::Lessthan: return std::make_shared<Bool>(m_now < time2->m_now);
    case TokenType::Greaterthan: return std::make_shared<Bool>(m_now > time2->m_now);
    case TokenType::Greaterthan_Equals: return std::make_shared<Bool>(m_now >= time2->m_now);
    case TokenType::Lessthan_Equals: return std::make_shared<Bool>(m_now <= time2->m_now);
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<Obj>
Time::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    auto time2 = dynamic_cast<Time *>(other);
    switch (op->type()) {
    case TokenType::Double_Equals: return std::make_shared<Bool>(m_now == time2->m_now);
    case TokenType::Bang_Equals: return std::make_shared<Bool>(m_now != time2->m_now);
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
}
