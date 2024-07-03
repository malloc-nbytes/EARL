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

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

List::List(std::vector<Obj *> value) : m_value(std::move(value)) {}

void List::fill(std::vector<Obj *> value) {
    m_value = std::move(value);
}

std::vector<Obj *> &List::value(void) {
    return m_value;
}

Type List::type(void) const {
    return Type::List;
}

Obj *List::nth(Obj *idx) {
    switch (idx->type()) {
    case Type::Int: {
        auto *index = dynamic_cast<Int *>(idx);
        if (index->value() < 0 || static_cast<size_t>(index->value()) > this->value().size()) {
            ERR_WARGS(Err::Type::Fatal, "index %d is out of range of length %zu",
                      index->value(), this->value().size());
        }
        return this->value()[index->value()];
    } break;
    default: assert(false && "invalid `nth` argument");
    }
    return nullptr; // unreachable
}

Obj *List::rev(void) {
    std::reverse(m_value.begin(), m_value.end());
    return new Void();
}

Obj *List::pop(Obj *idx) {
    assert(idx->type() == earl::value::Type::Int);
    earl::value::Int *index = dynamic_cast<earl::value::Int *>(idx);
    m_value.erase(m_value.begin() + index->value());
    return new Void();
}

Obj *List::append(std::vector<Obj *> &values) {
    // NOTE: To append a reference, use the below line
    // m_value.insert(m_value.end(), values.begin(), values.end());

    for (size_t i = 0; i < values.size(); ++i) {
        m_value.push_back(values[i]->copy());
    }

    return new Void();
}

Obj *List::filter(Obj *closure, Ctx &ctx) {
    assert(closure->type() == Type::Closure);
    Closure *cl = dynamic_cast<Closure *>(closure);

    List *copy = new List();
    std::vector<Obj *> keep_values;

    for (size_t i = 0; i < m_value.size(); ++i) {
        std::vector<Obj *> values = {m_value.at(i)};
        Obj *filter_result = cl->call(values, ctx);
        assert(filter_result->type() == Type::Bool);
        if (dynamic_cast<Bool *>(filter_result)->boolean()) {
            keep_values.push_back(m_value.at(i)->copy());
        }
    }

    copy->append(keep_values);

    return copy;
}

Obj *List::binop(Token *op, Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot binop (fix this message)");
    }

    Obj *tmp = other;
    switch (op->type()) {
    case TokenType::Plus: {
        auto list = new List(this->value());
        list->value().insert(list->value().end(),
                             dynamic_cast<List *>(tmp)->value().begin(),
                             dynamic_cast<List *>(tmp)->value().end());
        return list;
    } break;
    case TokenType::Double_Equals: {
        auto *other_lst = dynamic_cast<List *>(tmp);
        int res = 0;
        if (m_value.size() == other_lst->value().size()) {
            res = 1;
            for (size_t i = 0; i < m_value.size(); ++i) {
                Obj *o1 = this->value()[i];
                Obj *o2 = other_lst->value()[i];
                if (!type_is_compatable(o1, o2)) {
                    res = 0;
                    break;
                }
                if (o1->type() == Type::Int) {
                    if (dynamic_cast<Int *>(o1)->value() != dynamic_cast<Int *>(o2)->value()) {
                        res = 0;
                        break;
                    }
                }
                else if (o1->type() == Type::Str) {
                    if (dynamic_cast<Str *>(o1)->value() != dynamic_cast<Str *>(o2)->value()) {
                        res = 0;
                        break;
                    }
                }
                else {
                    res = 0;
                    break;
                }
            }
        }
        return new Int(res);
    } break;
    default: {
        Err::err_wtok(op);
        ERR(Err::Type::Fatal, "invalid binary operator");
    }
    }

    return nullptr; // unreachable
}

bool List::boolean(void) {
    return true;
}

void List::mutate(Obj *other) {
    (void)other;
    UNIMPLEMENTED("List::mutate");
}

Obj *List::copy(void) {
    auto list = new List();
    list->append(this->value());
    return list;
}

bool List::eq(Obj *other) {
    if (other->type() != Type::List)
        return false;

    auto *lst = dynamic_cast<List *>(other);

    if (lst->value().size() != this->value().size())
        return false;

    for (size_t i = 0; i < lst->value().size(); ++i) {
        if (!this->value()[i]->eq(lst->value()[i]))
            return false;
    }
    return true;
}
