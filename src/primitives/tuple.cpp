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

#include <algorithm>
#include <cassert>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Tuple::Tuple(std::vector<std::shared_ptr<Obj>> values) : m_values(values) {}

std::vector<std::shared_ptr<Obj>> &
Tuple::value(void) {
    return m_values;
}

std::shared_ptr<Obj>
Tuple::nth(std::shared_ptr<Obj> &idx) {
    switch (idx->type()) {
    case Type::Int: {
        auto index = dynamic_cast<Int *>(idx.get());
        if (index->value() < 0 || static_cast<size_t>(index->value()) > this->value().size()) {
            std::string msg = "index "+std::to_string(index->value())+" is out of range of length "+std::to_string(this->value().size());
            throw InterpreterException(msg);
        }
        return this->value().at(index->value());
    } break;
    default: {
        std::string msg = "invalid index when accessing value in a tuple";
        throw InterpreterException(msg);
    } break;
    }
    return nullptr; // unreachable
}

std::shared_ptr<Obj>
Tuple::back(void) {
    if (m_values.size() == 0)
        return std::make_shared<Option>();
    return m_values.back()->copy();
}

std::shared_ptr<Tuple>
Tuple::filter(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure.get());

    auto copy = std::make_shared<Tuple>();
    std::vector<std::shared_ptr<Obj>> keep_values = {};

    for (size_t i = 0; i < m_values.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_values.at(i)};
        std::shared_ptr<Obj> filter_result = cl->call(values, ctx);
        assert(filter_result->type() == Type::Bool);
        if (dynamic_cast<Bool *>(filter_result.get())->boolean())
            keep_values.push_back(m_values.at(i)->copy());
    }

    std::for_each(keep_values.begin(), keep_values.end(), [&](auto &v) {copy->m_values.push_back(v);});
    return copy;
}

void
Tuple::foreach(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure.get());
    for (size_t i = 0; i < m_values.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_values[i]};
        cl->call(values, ctx);
    }
}

std::shared_ptr<Tuple>
Tuple::rev(void) {
    auto tuple = std::make_shared<Tuple>();
    for (int i = m_values.size()-1; i >= 0; --i)
        tuple->m_values.push_back(m_values[i]->copy());
    return tuple;
}

/*** OVERRIDES ***/
Type
Tuple::type(void) const {
    return Type::Tuple;
}

std::shared_ptr<Obj>
Tuple::binop(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);

    auto other_tuple = dynamic_cast<Tuple *>(other.get());
    switch (op->type()) {
    case TokenType::Plus: {
        std::vector<std::shared_ptr<Obj>> values = {};
        std::for_each(m_values.begin(), m_values.end(), [&](auto &v){values.push_back(v);});
        std::for_each(other_tuple->value().begin(), other_tuple->value().end(), [&](auto &v){values.push_back(v);});
        return std::make_shared<Tuple>(values);
    } break;
    case TokenType::Double_Equals: {
        if (m_values.size() != other_tuple->value().size())
            return std::make_shared<Bool>(false);
        for (size_t i = 0; i < m_values.size(); ++i) {
            if (!m_values[i]->eq(other_tuple->value()[i]))
                return std::make_shared<Bool>(false);
        }
        return std::make_shared<Bool>(true);
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    }
    }
    assert(false && "unreachable");
    return nullptr;
}

bool
Tuple::boolean(void) {
    return true;
}

void
Tuple::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    (void)other;
    std::string msg = "unable to mutate value of type `tuple` as it is immutable";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Tuple::copy(void) {
    std::vector<std::shared_ptr<Obj>> values = {};
    std::for_each(m_values.begin(), m_values.end(), [&](auto &v){values.push_back(v);});
    return std::make_shared<Tuple>(values);
}

bool
Tuple::eq(std::shared_ptr<Obj> &other) {
    if (this->type() != other->type())
        return false;

    auto other_tuple = dynamic_cast<Tuple *>(other.get());
    if (m_values.size() != other_tuple->value().size())
        return false;

    for (size_t i = 0; i < m_values.size(); ++i) {
        if (!m_values[i]->eq(other_tuple->value()[i]))
            return false;
    }
    return true;
}

std::string
Tuple::to_cxxstring(void) {
    std::string res = "(";
    for (auto &el : m_values)
        res += el->to_cxxstring();
    res += ")";
    return res;
}

void
Tuple::spec_mutate(Token *op, const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    (void)other;
    Err::err_wtok(op);
    std::string msg = "unable to mutate value of type `tuple` as it is immutable";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Tuple::unaryop(Token *op) {
    (void)op;
    Err::err_wtok(op);
    std::string msg = "invalid unary operator for type `tuple`";
    throw InterpreterException(msg);
}
