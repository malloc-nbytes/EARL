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
#include <iostream>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Str::Str(std::string value) {
    m_value = value;
    m_chars = std::vector<std::shared_ptr<Char>>(value.size(), nullptr);
    m_changed = {};
    m_iterable = true;
}

Str::Str(std::vector<std::shared_ptr<Char>> chars) {
    assert(false && "unimplemented");
}

void
Str::update_changed(void) {
    for (int i : m_changed)
        m_value[i] = m_chars[i]->value();
    m_changed.clear();
}

std::shared_ptr<earl::value::Str>
Str::trim(Expr *expr) {
    size_t first = m_value.find_first_not_of(" \n\t");
    if (first == std::string::npos)
        return std::make_shared<earl::value::Str>("");
    size_t last = m_value.find_last_not_of(" \n\t");
    return std::make_shared<earl::value::Str>(m_value.substr(first, last-first+1));
}

std::string
Str::value(void) {
    this->update_changed();
    return m_value;
}

std::shared_ptr<Char>
Str::nth(Obj *idx, Expr *expr) {
    if (idx->type() != Type::Int) {
        Err::err_wexpr(expr);
        std::string msg = "invalid index when accessing value in a str";
        throw InterpreterException(msg);
    }

    auto index = dynamic_cast<Int *>(idx);
    int I = index->value();
    if (I < 0 || static_cast<size_t>(I) >= m_value.size()) {
        Err::err_wexpr(expr);
        std::string msg = "index "+std::to_string(index->value())+" is out of str range of length "+std::to_string(this->value().size());
        throw InterpreterException(msg);
    }

    if (m_chars.at(I)) {
        m_value.at(I) = m_chars.at(I)->value();
        return m_chars.at(I);
    }

    auto c = std::make_shared<Char>(m_value.at(I));
    m_chars.at(I) = std::move(c);
    m_changed.push_back(I);

    return m_chars.at(I);
}

// TODO: Adhere to new string optimization
std::shared_ptr<List>
Str::split(Obj *delim, Expr *expr) {
    if (delim->type() != Type::Str) {
        Err::err_wexpr(expr);
        const std::string msg = "cannot use member intrinsic `split` with non-str type";
        throw InterpreterException(msg);
    }

    this->update_changed();

    std::vector<std::shared_ptr<Obj>> splits = {};
    std::string delim_str = dynamic_cast<Str *>(delim)->value();
    std::string::size_type start = 0;

    auto pos = this->value().find(delim_str);
    std::string orig_value = this->value();

    while (pos != std::string::npos) {
        splits.push_back(std::make_shared<Str>(orig_value.substr(start, pos-start)));
        start = pos+delim_str.length();
        pos = orig_value.find(delim_str, start);
    }
    splits.push_back(std::make_shared<Str>(orig_value.substr(start)));

    return std::make_shared<List>(std::move(splits));
}

std::shared_ptr<Str>
Str::substr(Obj *idx1, Obj *idx2, Expr *expr) {
    if (idx1->type() != Type::Int || idx2->type() != Type::Int) {
        Err::err_wexpr(expr);
        const std::string msg = "cannot use member intrinsic `substr` with non-int types";
        throw InterpreterException(msg);
    }

    this->update_changed();

    int S = dynamic_cast<Int *>(idx1)->value();
    int N = dynamic_cast<Int *>(idx2)->value();

    return std::make_shared<Str>(m_value.substr(S, N));
}

void
Str::remove_char(int idx, Expr *expr) {
    if (idx >= m_value.size()) {
        Err::err_wexpr(expr);
        const std::string msg = "index "+std::to_string(idx)+" is out of range of length "+std::to_string(m_value.size());
        throw InterpreterException(msg);
    }

    this->update_changed();
    m_value.erase(m_value.begin() + idx);
    m_chars.erase(m_chars.begin() + idx);
}

void
Str::pop(Obj *idx, Expr *expr) {
    auto *idx1 = dynamic_cast<earl::value::Int *>(idx);
    int I = idx1->value();
    this->remove_char(I, expr);

    // if (I >= m_value.size()) {
    //     Err::err_wexpr(expr);
    //     const std::string msg = "index "+std::to_string(I)+" is out of range of length "+std::to_string(m_value.size());
    //     throw InterpreterException(msg);
    // }

    // this->update_changed();
    // m_value.erase(m_value.begin() + I);
    // m_chars.erase(m_chars.begin() + I);
}

std::shared_ptr<Obj>
Str::back(void) {
    this->update_changed();

    if (m_value.size() == 0)
        return std::make_shared<Option>();

    if (m_chars.back()) {
        m_value.back() = m_chars.back()->value();
        return m_chars.back();
    }

    auto c = std::make_shared<Char>(m_value.back());
    m_chars.back() = std::move(c);
    return m_chars.back();
}

std::shared_ptr<Str>
Str::rev(void) {
    this->update_changed();
    auto str = std::make_shared<Str>();
    for (int i = m_value.size()-1; i >= 0; --i)
        str->append(m_value[i]);
    return str;
}

void
Str::append(const std::string &value) {
    for (size_t i = 0; i < value.size(); ++i) {
        m_value.push_back(value.at(i));
        m_chars.push_back(nullptr);
    }
}

void
Str::append(char c) {
    m_value.push_back(c);
    m_chars.push_back(nullptr);
}

void
Str::append(Obj *c) {
    if (c->type() == Type::Char) {
        auto cx = dynamic_cast<Char *>(c);
        m_value.push_back(cx->value());
        m_chars.push_back(nullptr);
    }
    else {
        auto s = dynamic_cast<Str *>(c);
        m_value += s->value();
        for (int i=0; i < s->value().size(); ++i)
            m_chars.push_back(nullptr);
    }
}

void
Str::append(std::vector<std::shared_ptr<Obj>> &values, Expr *expr) {
    for (size_t i = 0; i < values.size(); ++i) {
        if (!type_is_compatable(this, values[i].get())) {
            Err::err_wexpr(expr);
            const std::string msg = "type str is incompatible with type `"+earl::value::type_to_str(values.at(i)->type())+"`";
            throw InterpreterException(msg);
        }
        this->append(values[i].get());
    }
}

std::shared_ptr<Str>
Str::filter(Obj *closure, std::shared_ptr<Ctx> &ctx) {
    this->update_changed();

    // Closure *cl = dynamic_cast<Closure *>(closure);
    void *cl = closure;

    auto acc = std::make_shared<Str>();

    for (int i = 0; i < m_value.size(); ++i) {
        std::shared_ptr<Char> cx = nullptr;
        if (m_chars.at(i)) {
            m_value.at(i) = m_chars.at(i)->value();
            cx = m_chars.at(i);
        }
        else {
            auto tmpc = std::make_shared<Char>(m_value.at(i));
            m_chars.at(i) = std::move(tmpc);
            cx = m_chars.at(i);
            m_changed.push_back(i);
        }
        if (closure->type() == earl::value::Type::Closure) {
            std::vector<std::shared_ptr<Obj>> values = {cx};
            std::shared_ptr<Obj> filter_result = ((Closure *)cl)->call(values, ctx);
            if (dynamic_cast<Bool *>(filter_result.get())->boolean())
                acc->append(cx.get());
        }
        else { // Predicate
            std::shared_ptr<Obj> filter_result = ((Predicate *)cl)->check(cx.get(), ctx);
            if (dynamic_cast<Bool *>(filter_result.get())->boolean())
                acc->append(cx.get());
        }
    }

    return acc;
}

std::shared_ptr<Bool>
Str::contains(Char *value) {
    for (size_t i = 0; i < m_value.size(); ++i) {
        if (m_chars.at(i) && m_chars.at(i)->value() != m_value.at(i))
            m_value.at(i) = m_chars.at(i)->value();

        if (m_value.at(i) == value->value())
            return std::make_shared<Bool>(true);
    }
    return std::make_shared<Bool>(false);
}

void
Str::foreach(Obj *closure, std::shared_ptr<Ctx> &ctx) {
    this->update_changed();
    Closure *cl = dynamic_cast<Closure *>(closure);
    for (size_t i = 0; i < m_value.size(); ++i) {
        std::shared_ptr<Char> cx = nullptr;
        if (m_chars.at(i)) {
            m_value.at(i) = m_chars.at(i)->value();
            cx = m_chars.at(i);
        }
        else {
            auto tmpc = std::make_shared<Char>(m_value.at(i));
            m_chars.at(i) = std::move(tmpc);
            cx = m_chars.at(i);
            m_changed.push_back(i);
        }
        std::vector<std::shared_ptr<Obj>> values = {cx};
        cl->call(values, ctx);
    }
}

Type
Str::type(void) const {
    return Type::Str;
}

std::shared_ptr<Obj>
Str::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    this->update_changed();
    switch (op->type()) {
    case TokenType::Plus: {
        return std::make_shared<Str>(this->value() + dynamic_cast<Str *>(other)->value());
    } break;
    case TokenType::Double_Equals: {
        return std::make_shared<Bool>(this->value() == dynamic_cast<Str *>(other)->value());
    } break;
    case TokenType::Bang_Equals: {
        return std::make_shared<Bool>(this->value() != dynamic_cast<Str *>(other)->value());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    }
    }
}

bool
Str::boolean(void) {
    return m_value.size() > 0;
}

std::vector<std::shared_ptr<Char>>
Str::value_as_earlchar(void) {
    std::vector<std::shared_ptr<Char>> values = {};
    std::for_each(m_value.begin(), m_value.end(), [&](char c){
        auto cx = std::make_shared<Char>(c);
        values.push_back(std::move(cx));
    });
    return values;
}

std::shared_ptr<Char>
Str::__get_elem(size_t idx) {
    this->update_changed();
    int I = idx;
    std::shared_ptr<Char> c = nullptr;
    if (m_chars.at(I)) {
        m_value.at(I) = m_chars.at(I)->value();
        c = m_chars.at(I);
    }
    else {
        auto tmpc = std::make_shared<Char>(m_value.at(I));
        m_chars.at(I) = std::move(tmpc);
        c = m_chars.at(I);
        m_changed.push_back(I);
    }
    return c;
}

void
Str::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    if (other->type() == earl::value::Type::Str) {
        Str *otherstr = dynamic_cast<Str *>(other);
        m_value = otherstr->m_value;
        m_chars = otherstr->m_chars;
    }
    else if (other->type() == earl::value::Type::Char) {
        Char *otherchar = dynamic_cast<Char *>(other);
        m_value.clear();
        m_chars.clear();
        m_value.push_back(otherchar->value());
        m_chars.push_back(nullptr);
    }
    else
        assert(false && "unreachable");
}

std::shared_ptr<Obj>
Str::copy(void) {
    this->update_changed();
    auto value = std::make_shared<Str>(m_value);
    value->set_owner(m_var_owner);
    return value;
}

bool
Str::eq(Obj *other) {
    if (other->type() != Type::Str)
        return false;
    return this->value() == dynamic_cast<Str *>(other)->value();
}

std::string
Str::to_cxxstring(void) {
    return this->value();
}

void
Str::spec_mutate(Token *op, Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    this->update_changed();

    switch (op->type()) {
    case TokenType::Plus_Equals: {
        this->append(other);
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid operator for special mutation `"+op->lexeme()+"` on str type";
        throw InterpreterException(msg);
    } break;
    }
}

Iterator
Str::iter_begin(void) {
    auto it = m_chars.begin();

    if (!*it) {
        auto c = std::make_shared<Char>(m_value.at(0));
        *it = std::move(c);
        m_changed.push_back(0);
    }

    return it;
}

Iterator
Str::iter_end(void) {
    return m_chars.end();
}

void
Str::iter_next(Iterator &it) {
    std::visit([&](auto &iter) {
        using IteratorType = std::decay_t<decltype(iter)>;

        if constexpr (std::is_same_v<IteratorType, std::vector<std::shared_ptr<Char>>::iterator>) {
            std::advance(iter, 1);

            if (iter == m_chars.end())
                return;

            size_t index = std::distance(m_chars.begin(), iter);

            if (index < m_value.size()) {
                auto c = std::make_shared<Char>(m_value.at(index));
                *iter = std::move(c);
                m_changed.push_back(index);
            }
        }
        // No else case needed since we only handle std::vector<std::shared_ptr<Char>>::iterator
    }, it);
}

std::shared_ptr<Obj>
Str::add(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    if (other->type() == Type::Char)
        return std::make_shared<Str>(this->value() + dynamic_cast<Char *>(other)->value());
    return std::make_shared<Str>(this->value() + dynamic_cast<Str *>(other)->value());
}

std::shared_ptr<Obj>
Str::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);
    switch (op->type()) {
    case TokenType::Double_Equals: {
        if (other->type() == Type::Char) {
            auto ch = dynamic_cast<Char *>(other);
            if (this->value().size() != 1)
                return std::make_shared<Bool>(false);
            return std::make_shared<Bool>(this->value()[0] == ch->value());
        }
        return std::make_shared<Bool>(this->value() == dynamic_cast<Str *>(other)->value());
    } break;
    case TokenType::Bang_Equals: {
        if (other->type() == Type::Char) {
            auto ch = dynamic_cast<Char *>(other);
            if (this->value().size() != 1)
                return std::make_shared<Bool>(true);
            return std::make_shared<Bool>(this->value()[0] != ch->value());
        }
        return std::make_shared<Bool>(this->value() != dynamic_cast<Str *>(other)->value());
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid binary operator";
        throw InterpreterException(msg);
    }
    }
    return nullptr; // unreachable
}
