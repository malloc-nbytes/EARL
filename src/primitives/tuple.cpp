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
#include <algorithm>
#include <cassert>
#include <memory>

#include "earl.hpp"
#include "err.hpp"

using namespace earl::value;

Tuple::Tuple(std::vector<std::shared_ptr<Obj>> values) : m_values(values) {
    m_iterable = true;
}

std::vector<std::shared_ptr<Obj>> &
Tuple::value(void) {
    return m_values;
}

std::shared_ptr<Obj>
Tuple::nth(Obj *idx, Expr *expr) {
    switch (idx->type()) {
    case Type::Int: {
        auto index = dynamic_cast<Int *>(idx);
        if (index->value() < 0 || static_cast<size_t>(index->value()) >= this->value().size()) {
            Err::err_wexpr(expr);
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
Tuple::filter(Obj *closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure);

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
Tuple::foreach(Obj *closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure);
    for (size_t i = 0; i < m_values.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_values[i]};
        cl->call(values, ctx);
    }
}

std::shared_ptr<Bool>
Tuple::contains(Obj *value) {
    for (size_t i = 0; i < m_values.size(); ++i)
        if (m_values.at(i)->eq(value))
            return std::make_shared<Bool>(true);
    return std::make_shared<Bool>(false);
}

std::shared_ptr<Tuple>
Tuple::rev(void) {
    auto tuple = std::make_shared<Tuple>();
    for (int i = m_values.size()-1; i >= 0; --i)
        tuple->m_values.push_back(m_values.at(i)->copy());
    return tuple;
}

std::shared_ptr<Obj>
Tuple::fold(Closure *closure, std::shared_ptr<Obj> acc, std::shared_ptr<Ctx> &ctx) {
    for (size_t i = 0; i < m_values.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_values.at(i), acc};
        acc = closure->call(values, ctx);
    }

    return acc;
}

/*** OVERRIDES ***/
Type
Tuple::type(void) const {
    return Type::Tuple;
}

std::shared_ptr<Obj>
Tuple::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    auto other_tuple = dynamic_cast<Tuple *>(other);
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
            if (!m_values[i]->eq(other_tuple->value()[i].get()))
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
    return m_values.size() > 0;
}

std::shared_ptr<Obj>
Tuple::copy(void) {
    std::vector<std::shared_ptr<Obj>> values = {};
    std::for_each(m_values.begin(), m_values.end(), [&](auto &v){values.push_back(v);});
    auto value = std::make_shared<Tuple>(values);
    value->set_owner(m_var_owner);
    return value;
}

bool
Tuple::eq(Obj *other) {
    if (this->type() != other->type())
        return false;

    auto other_tuple = dynamic_cast<Tuple *>(other);
    if (m_values.size() != other_tuple->value().size())
        return false;

    for (size_t i = 0; i < m_values.size(); ++i) {
        if (!m_values[i]->eq(other_tuple->value()[i].get()))
            return false;
    }
    return true;
}

std::string
Tuple::to_cxxstring(void) {
    std::string res = "(";
    for (size_t i = 0; i < m_values.size(); ++i) {
        res += m_values.at(i)->to_cxxstring();
        if (i != m_values.size()-1)
            res += ", ";
    }
    res += ")";
    return res;
}

Iterator
Tuple::iter_begin(void) {
    return m_values.begin();
}

Iterator
Tuple::iter_end(void) {
    return m_values.end();
}

void
Tuple::iter_next(Iterator &it) {
    std::visit([&](auto &iter) {
        std::advance(iter, 1);
    }, it);
}

std::shared_ptr<Obj>
Tuple::add(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    auto other_tuple = dynamic_cast<Tuple *>(other);
    std::vector<std::shared_ptr<Obj>> values = {};
    std::for_each(m_values.begin(), m_values.end(), [&](auto &v){values.push_back(v);});
    std::for_each(other_tuple->value().begin(), other_tuple->value().end(), [&](auto &v){values.push_back(v);});
    return std::make_shared<Tuple>(values);
}

std::shared_ptr<Obj>
Tuple::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    auto other_tuple = dynamic_cast<Tuple *>(other);
    switch (op->type()) {
    case TokenType::Double_Equals: {
        if (m_values.size() != other_tuple->value().size())
            return std::make_shared<Bool>(false);
        for (size_t i = 0; i < m_values.size(); ++i) {
            if (!m_values[i]->eq(other_tuple->value()[i].get()))
                return std::make_shared<Bool>(false);
        }
        return std::make_shared<Bool>(true);
    } break;
    case TokenType::Bang_Equals: {
        UNIMPLEMENTED("Tuple::equality:TokenType::Bang_Equals");
    } break;
    default: {
        Err::err_wtok(op);
        const std::string msg = "invalid operator";
        throw InterpreterException(msg);
    } break;
    }
    return nullptr; // unreachable
}


