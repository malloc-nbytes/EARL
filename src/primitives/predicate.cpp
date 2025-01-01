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

Predicate::Predicate(Token *op, std::shared_ptr<Obj> right)
    : m_op(op), m_right(right) {}

Type
Predicate::type(void) const {
    return Type::Predicate;
}

std::string
Predicate::to_cxxstring(void) {
    return "< Predicate {" + m_op->lexeme() + ' ' + m_right->to_cxxstring() + "} >";
}

std::shared_ptr<Obj>
Predicate::copy(void) {
    return std::make_shared<Predicate>(m_op, m_right);
}

std::shared_ptr<Obj>
Predicate::check(Obj *obj, std::shared_ptr<Ctx> &ctx) const {
    switch (m_op->type()) {
    case TokenType::Lessthan:
    case TokenType::Greaterthan:
    case TokenType::Lessthan_Equals:
    case TokenType::Greaterthan_Equals: {
        return obj->gtequality(m_op, m_right.get());
    } break;
    case TokenType::Double_Equals:
    case TokenType::Bang_Equals: {
        return obj->equality(m_op, m_right.get());
    } break;
    default: {
        assert(false && "unreachable");
    } break;
    }

    assert(false);
}
