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

List::List(std::vector<std::shared_ptr<Obj>> value)
    : m_value(value) {}

void List::fill(std::vector<std::shared_ptr<Obj>> &value) {
    (void)value;
    UNIMPLEMENTED("List::fill");
}

std::vector<std::shared_ptr<Obj>> &
List::value(void) {
    return m_value;
}

Type
List::type(void) const {
    return Type::List;
}

std::shared_ptr<Obj>
List::nth(std::shared_ptr<Obj> &idx) {
    switch (idx->type()) {
    case Type::Int: {
        auto index = dynamic_cast<Int *>(idx.get());
        if (index->value() < 0 || static_cast<size_t>(index->value()) > this->value().size()) {
            ERR_WARGS(Err::Type::Fatal, "index %d is out of range of length %zu",
                      index->value(), this->value().size());
        }
        return this->value().at(index->value());
    } break;
    default: ERR(Err::Type::Fatal, "invalid index when accessing value in a list");
    }
    return nullptr; // unreachable
}

void
List::rev(void) {
    for (size_t i = 0; i < this->value().size()/2; ++i) {
        auto copy = this->value().at(i);
        this->value().at(i) = this->value().at(this->value().size()-i-1);
        this->value().at(this->value().size()-i-1) = copy;
    }
}

void
List::pop(std::shared_ptr<Obj> &idx) {
    auto *idx1 = dynamic_cast<earl::value::Int *>(idx.get());
    m_value.erase(m_value.begin() + idx1->value());
}

void
List::append(std::vector<std::shared_ptr<Obj>> values) {
    for (size_t i = 0; i < values.size(); ++i) {
        m_value.push_back(values[i]->copy());
    }
}

void
List::append(std::shared_ptr<Obj> value) {
    m_value.push_back(value);
}

std::shared_ptr<List>
List::filter(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure.get());

    auto copy = std::make_shared<List>();
    std::vector<std::shared_ptr<Obj>> keep_values={};

    for (size_t i = 0; i < m_value.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_value.at(i)};
        std::shared_ptr<Obj> filter_result = cl->call(values, ctx);
        assert(filter_result->type() == Type::Bool);
        if (dynamic_cast<Bool *>(filter_result.get())->boolean())
            keep_values.push_back(m_value.at(i)->copy());
    }

    copy->append(keep_values);

    return copy;
}

void
List::foreach(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure.get());
    for (size_t i = 0; i < m_value.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_value[i]};
        cl->call(values, ctx);
    }
}

std::shared_ptr<Obj>
List::back(void) {
    if (m_value.size() == 0)
        return std::make_shared<Option>();
    return m_value.back()->copy();
}

std::shared_ptr<Obj>
List::binop(Token *op, std::shared_ptr<Obj> &other) {
    if (!type_is_compatable(this, other.get()))
        assert(false && "cannot binop (fix this message)");

    auto other_casted = dynamic_cast<List *>(other.get());

    switch (op->type()) {
    case TokenType::Plus: {
        auto list = std::make_shared<List>(this->value());
        list->value().insert(list->value().end(),
                             other_casted->value().begin(),
                             other_casted->value().end());
        return list;
    } break;
    case TokenType::Double_Equals: {
        int res = 0;
        if (m_value.size() == other_casted->value().size()) {
            res = 1;
            for (size_t i = 0; i < m_value.size(); ++i) {
                auto o1 = this->value()[i];
                auto o2 = other_casted->value()[i];
                if (!type_is_compatable(o1.get(), o2.get())) {
                    res = 0;
                    break;
                }
                if (o1->type() == Type::Int) {
                    if (dynamic_cast<Int *>(o1.get())->value() != dynamic_cast<Int *>(o2.get())->value()) {
                        res = 0;
                        break;
                    }
                }
                else if (o1->type() == Type::Str) {
                    if (dynamic_cast<Str *>(o1.get())->value() != dynamic_cast<Str *>(o2.get())->value()) {
                        res = 0;
                        break;
                    }
                }
                else if (o1->type() == Type::Char) {
                    if (dynamic_cast<Char *>(o1.get())->value() != dynamic_cast<Char *>(o2.get())->value()) {
                        res = 0;
                        break;
                    }
                }
                else if (o1->type() == Type::Bool) {
                    if (dynamic_cast<Bool *>(o1.get())->value() != dynamic_cast<Bool *>(o2.get())->value()) {
                        res = 0;
                        break;
                    }
                }
                else if (o1->type() == Type::Option) {
                    auto option1 = dynamic_cast<Option *>(o1.get());
                    auto option2 = dynamic_cast<Option *>(o1.get());
                    if ((!option1 && option2) || (option1 && !option2)) {
                        res = 0;
                        break;
                    }
                    if ((option1 && option2) && (option1->value() != option2->value())) {
                        res = 0;
                        break;
                    }
                }
                else {
                    assert(false && "unreachable");

                    // res = 0;
                    // break;
                }
            }
        }
        return std::make_shared<Int>(res);
    } break;
    default: {
        Err::err_wtok(op);
        ERR(Err::Type::Fatal, "invalid binary operator");
    }
    }

    return nullptr; // unreachable
}

bool
List::boolean(void) {
    return m_value.size() > 0;
}

void
List::mutate(const std::shared_ptr<Obj> &other) {
    if (!type_is_compatable(this, other.get()))
        assert(false && "cannot mutate (fix this message)");
    auto *lst = dynamic_cast<List *>(other.get());
    m_value = lst->value();
}

std::shared_ptr<Obj>
List::copy(void) {
    auto list = std::make_shared<List>();
    list->append(this->value());
    return list;
}

bool
List::eq(std::shared_ptr<Obj> &other) {
    if (other->type() != Type::List)
        return false;

    auto *lst = dynamic_cast<List *>(other.get());

    if (lst->value().size() != this->value().size())
        return false;

    for (size_t i = 0; i < lst->value().size(); ++i)
        if (!this->value()[i]->eq(lst->value()[i]))
            return false;

    return true;
}

std::string
List::to_cxxstring(void) {
    std::string res = "";
    for (auto &el : m_value)
        res += el->to_cxxstring();
    return res;
}

void
List::spec_mutate(Token *op, const std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("List::spec_mutate");
}

