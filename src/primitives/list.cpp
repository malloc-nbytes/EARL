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

#include <iterator>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

List::List(std::vector<std::shared_ptr<Obj>> value)
    : m_value(value) {
    m_iterable = true;
}

std::vector<std::shared_ptr<Obj>> &
List::value(void) {
    return m_value;
}

Type
List::type(void) const {
    return Type::List;
}

std::vector<std::shared_ptr<Obj>>
List::slice(Obj *start, Obj *end, Expr *expr) {
    if (start->type() != Type::Void && start->type() != Type::Int) {
        Err::err_wexpr(expr);
        std::string msg = "invalid slice `start` type: `"+type_to_str(start->type())+"`";
        throw InterpreterException(msg);
    }
    if (end->type() != Type::Void && end->type() != Type::Int) {
        Err::err_wexpr(expr);
        std::string msg = "invalid slice `end` type: `"+type_to_str(end->type())+"`";
        throw InterpreterException(msg);
    }

    std::vector<std::shared_ptr<Obj>> v = {};
    if (start->type() == Type::Void && end->type() == Type::Void) {
        std::for_each(m_value.begin(), m_value.end(), [&](auto &k) {v.push_back(k);});
        return v;
    }
    int s, e;

    if (start->type() == Type::Void)
        s = 0;
    else
        s = dynamic_cast<Int *>(start)->value();

    if (end->type() == Type::Void)
        e = m_value.size();
    else
        e = dynamic_cast<Int *>(end)->value();

    for (; s < e; ++s) {
        if (s >= m_value.size()) {
            Err::err_wexpr(expr);
            std::string msg = "index "+std::to_string(s)+" is out of range for list of length "+std::to_string(m_value.size());
            throw InterpreterException(msg);
        }
        v.push_back(m_value.at(s));
    }

    return v;
}

std::shared_ptr<Obj>
List::nth(std::shared_ptr<Obj> &idx, Expr *expr) {
    switch (idx->type()) {
    case Type::Int: {
        auto index = dynamic_cast<Int *>(idx.get());
        if (index->value() < 0 || static_cast<size_t>(index->value()) >= this->value().size()) {
            Err::err_wexpr(expr);
            std::string msg = "index "+std::to_string(index->value())+" is out of range of length "+std::to_string(this->value().size());
            throw InterpreterException(msg);
        }
        return this->value().at(index->value());
    } break;
    case Type::Slice: {
        auto slice = dynamic_cast<Slice *>(idx.get());
        std::shared_ptr<Obj> &s = slice->start(), &e = slice->end();
        return std::make_shared<List>(this->slice(s.get(), e.get(), expr));
    } break;
    default: {
        Err::err_wexpr(expr);
        std::string msg = "invalid index value when accessing value in a list";
        throw InterpreterException(msg);
    }
    }
    return nullptr; // unreachable
}

std::shared_ptr<List>
List::rev(void) {
    auto lst = std::make_shared<List>();
    for (int i = m_value.size()-1; i >= 0; --i)
        lst->append(m_value[i]);
    return lst;
}

std::shared_ptr<Bool>
List::contains(Obj *value) {
    for (size_t i = 0; i < m_value.size(); ++i)
        if (m_value.at(i)->eq(value))
            return std::make_shared<Bool>(true);
    return std::make_shared<Bool>(false);
}

void
List::pop(Obj *idx, Expr *expr) {
    auto *idx1 = dynamic_cast<earl::value::Int *>(idx);

    if (idx1->value() >= m_value.size()) {
        Err::err_wexpr(expr);
        const std::string msg = "index "
            +std::to_string(idx1->value())
            +" is out of range of length "
            +std::to_string(m_value.size());
        throw InterpreterException(msg);
    }

    m_value.erase(m_value.begin() + idx1->value());
}

void
List::append(std::vector<std::shared_ptr<Obj>> &values) {
    for (size_t i = 0; i < values.size(); ++i) {
        m_value.push_back(values.at(i));
    }
}

void
List::append(std::shared_ptr<Obj> value) {
    m_value.push_back(value);
}

void
List::append_copy(std::vector<std::shared_ptr<Obj>> &values) {
    for (size_t i = 0; i < values.size(); ++i) {
        m_value.push_back(values.at(i)->copy());
    }
}

void
List::append_copy(std::shared_ptr<Obj> value) {
    m_value.push_back(value->copy());
}

std::shared_ptr<List>
List::filter(Obj *closure, std::shared_ptr<Ctx> &ctx) {
    auto copy = std::make_shared<List>();
    std::vector<std::shared_ptr<Obj>> keep_values={};

    if (closure->type() == earl::value::Type::Closure) {
        Closure *cl = dynamic_cast<Closure *>(closure);
        for (size_t i = 0; i < m_value.size(); ++i) {
            std::vector<std::shared_ptr<Obj>> values = {m_value.at(i)};
            std::shared_ptr<Obj> filter_result = cl->call(values, ctx);
            if (dynamic_cast<Bool *>(filter_result.get())->boolean())
                keep_values.push_back(m_value.at(i)->copy());
        }

    }
    else {
        Predicate *pred = dynamic_cast<Predicate *>(closure);
        for (size_t i = 0; i < m_value.size(); ++i) {
            std::shared_ptr<Obj> filter_result = pred->check(m_value.at(i).get(), ctx);
            if (dynamic_cast<Bool *>(filter_result.get())->boolean())
                keep_values.push_back(m_value.at(i)->copy());
        }
    }

    copy->append(keep_values);
    return copy;
}

std::shared_ptr<Obj>
List::fold(Closure *closure, std::shared_ptr<Obj> acc, std::shared_ptr<Ctx> &ctx) {
    for (size_t i = 0; i < m_value.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_value.at(i), acc};
        acc = closure->call(values, ctx);
    }

    return acc;
}

void
List::foreach(Obj *closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure);
    for (size_t i = 0; i < m_value.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_value[i]};
        cl->call(values, ctx);
    }
}

std::shared_ptr<List>
List::map(Closure *closure, std::shared_ptr<Ctx> &ctx) {
    auto mapped = std::make_shared<List>();
    for (size_t i = 0; i < m_value.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> params = {m_value.at(i)};
        auto value = closure->call(params, ctx);
        mapped->append(value);
    }
    return mapped;
}

std::shared_ptr<Obj>
List::back(void) {
    if (m_value.size() == 0)
        return std::make_shared<Option>();
    return m_value.back()->copy();
}

std::shared_ptr<Obj>
List::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    auto other_casted = dynamic_cast<List *>(other);

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
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    }
    }

    return nullptr; // unreachable
}

bool
List::boolean(void) {
    return m_value.size() > 0;
}

void
List::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    auto *lst = dynamic_cast<List *>(other);
    m_value = lst->value();
}

std::shared_ptr<Obj>
List::copy(void) {
    auto list = std::make_shared<List>();
    list->append_copy(this->value());
    list->set_owner(m_var_owner);
    return list;
}

bool
List::eq(Obj *other) {
    if (other->type() != Type::List)
        return false;

    auto *lst = dynamic_cast<List *>(other);

    if (lst->value().size() != this->value().size())
        return false;

    for (size_t i = 0; i < lst->value().size(); ++i)
        if (!this->value()[i]->eq(lst->value()[i].get()))
            return false;

    return true;
}

std::string
List::to_cxxstring(void) {
    std::string res = "[";
    for (size_t i = 0; i < m_value.size(); ++i) {
        res += m_value.at(i)->to_cxxstring();
        if (i != m_value.size()-1)
            res += ", ";
    }
    res += "]";
    return res;
}

void
List::spec_mutate(Token *op, Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    switch (op->type()) {
    case TokenType::Plus_Equals: {
        auto otherlst = dynamic_cast<const List *>(other);
        m_value.insert(m_value.end(), otherlst->m_value.begin(), otherlst->m_value.end());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid operator for special mutation `"+op->lexeme()+"` on list type";
        throw InterpreterException(msg);
    } break;
    }
}

Iterator
List::iter_begin(void) {
    return m_value.begin();
}

Iterator
List::iter_end(void) {
    return m_value.end();
}

void
List::iter_next(Iterator &it) {
    std::visit([&](auto &iter) {
        std::advance(iter, 1);
    }, it);
}

std::shared_ptr<Obj>
List::add(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    auto other_casted = dynamic_cast<List *>(other);
    auto list = std::make_shared<List>(this->value());
    list->value().insert(list->value().end(),
                         other_casted->value().begin(),
                         other_casted->value().end());
    return list;
}

std::shared_ptr<Obj>
List::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    auto other_casted = dynamic_cast<List *>(other);

    if (op->type() == TokenType::Double_Equals) {
        if (m_value.size() != other_casted->value().size())
            return std::make_shared<Bool>(false);

        for (size_t i = 0; i < m_value.size(); ++i)
            if (!m_value[i]->eq(other_casted->value()[i].get()))
                return std::make_shared<Bool>(false);

        return std::make_shared<Bool>(true);
    }
    else if (op->type() == TokenType::Bang_Equals) {
        if (m_value.size() != other_casted->value().size())
            return std::make_shared<Bool>(true);

        for (size_t i = 0; i < m_value.size(); ++i)
            if (!m_value[i]->eq(other_casted->value()[i].get()))
                return std::make_shared<Bool>(true);

        return std::make_shared<Bool>(false);
    }

    Err::err_wtok(op);
    const std::string msg = "invalid operator";
    throw InterpreterException(msg);
}
