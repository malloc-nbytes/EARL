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
#include "utils.hpp"

using namespace earl::value;

Str::Str(std::string value) {
    m_value = value;
    m_chars = {};
    for (size_t i = 0; i < m_value.size(); ++i)
        m_chars.push_back(nullptr);
    // bool escape = false;
    // for (size_t i = 0; i < value.size(); ++i) {
    //     char c = value[i];
    //     if (!escape && c == '\\') {
    //         escape = true;
    //         continue;
    //     }
    //     if (escape) {
    //         switch (c) {
    //         case 'n': {
    //             m_value.push_back(std::make_shared<Char>(std::string(1, '\n')));
    //         } break;
    //         case 't': {
    //             m_value.push_back(std::make_shared<Char>(std::string(1, '\t')));
    //         } break;
    //         case '"': {
    //             m_value.push_back(std::make_shared<Char>(std::string(1, '"')));
    //         } break;
    //         case '\\': {
    //             m_value.push_back(std::make_shared<Char>(std::string(1, '\\')));
    //         } break;
    //         default:
    //             std::string msg = "unkown escape sequence `\\"+std::to_string(c)+"`";
    //             throw InterpreterException(msg);
    //         }
    //         escape = false;
    //     }
    //     else {
    //         m_value.push_back(std::make_shared<Char>(std::string(1, c)));
    //     }
    // }
}

Str::Str(std::vector<std::shared_ptr<Char>> chars) {
    // std::for_each(chars.begin(), chars.end(), [&](auto &c){m_value.push_back(c);});
    assert(false && "unimplemented");
}

std::string
Str::value(void) {
    std::string actual = "";
    for (size_t i = 0; i < m_value.size(); ++i) {
        if (m_value.at(i) == '\0')
            actual += m_chars.at(i)->value();
        else
            actual += m_value.at(i);
    }
    return actual;
}

std::shared_ptr<Char>
Str::nth(std::shared_ptr<Obj> &idx) {
    if (idx->type() != Type::Int) {
        std::string msg = "invalid index when accessing value in a list";
        throw InterpreterException(msg);
    }

    auto index = dynamic_cast<Int *>(idx.get());
    if (index->value() < 0 || static_cast<size_t>(index->value()) >= m_value.size()) {
        std::string msg = "index "+std::to_string(index->value())+" is out of str range of length "+std::to_string(this->value().size());
        throw InterpreterException(msg);
    }

    if (m_value.at(index->value()) == '\0')
        return m_chars.at(index->value());

    auto c = std::make_shared<Char>(std::string(1, m_value[index->value()]));
    m_chars.at(index->value()) = std::move(c);
    m_value.at(index->value()) = '\0';
    return m_chars.at(index->value());
}

// CHANGEME
std::shared_ptr<List>
Str::split(std::shared_ptr<Obj> &delim) {
    if (delim->type() != Type::Str) {
        const std::string msg = "cannot use member intrinsic `split` with non-str type";
        throw InterpreterException(msg);
    }

    std::vector<std::shared_ptr<Obj>> splits = {};
    std::string delim_str = dynamic_cast<Str *>(delim.get())->value();
    std::string::size_type start = 0;

    auto pos = this->value().find(delim_str);

    while (pos != std::string::npos) {
        splits.push_back(std::make_shared<Str>(m_value.substr(start, pos-start)));
        start = pos+delim_str.length();
        pos = m_value.find(delim_str, start);
    }
    splits.push_back(std::make_shared<Str>(m_value.substr(start)));

    return std::make_shared<List>(std::move(splits));
}

// CHANGEME
std::shared_ptr<Str>
Str::substr(std::shared_ptr<Obj> &idx1, std::shared_ptr<Obj> &idx2) {
    if (idx1->type() != Type::Int || idx2->type() != Type::Int) {
        const std::string msg = "cannot use member intrinsic `substr` with non-int types";
        throw InterpreterException(msg);
    }
    std::string sub = m_value.substr(dynamic_cast<Int *>(idx1.get())->value(), dynamic_cast<Int*>(idx2.get())->value());
    return std::make_shared<Str>(sub);
}

void
Str::pop(std::shared_ptr<Obj> &idx) {
    auto *idx1 = dynamic_cast<earl::value::Int *>(idx.get());
    int I = idx1->value();

    if (m_value.at(I) == '\0') {
        m_chars.erase(m_chars.begin() + I);
    }
    m_value.erase(m_value.begin() + I);
}

std::shared_ptr<Obj>
Str::back(void) {
    if (m_value.size() == 0)
        return std::make_shared<Option>();
    if (m_value.back() == '\0')
        return m_chars.back();

    auto c = std::make_shared<Char>(std::string(1, m_value.back()));
    m_chars.at(m_chars.size()-1) = std::move(c);
    m_value.at(m_value.size()-1) = '\0';
    return m_chars.at(m_chars.size()-1);
}

std::shared_ptr<Str>
Str::rev(void) {
    auto str = std::make_shared<Str>();
    for (int i = m_value.size()-1; i >= 0; --i)
        str->append(m_value[i]);
    return str;
}

void
Str::append(char c) {
    m_value.push_back(c);
    m_chars.push_back(nullptr);
}

void
Str::append(std::shared_ptr<Obj> c) {
    if (c->type() == Type::Char) {
        auto cx = std::dynamic_pointer_cast<Char>(c);
        m_chars.push_back(cx);
        m_value.push_back('\0');
    }
    else {
        auto s = dynamic_cast<Str *>(c.get());
        for (auto &cx : s->value())
            m_value.push_back(cx);
    }
}

void
Str::append(std::vector<std::shared_ptr<Obj>> &values) {
    for (size_t i = 0; i < values.size(); ++i) {
        if (!type_is_compatable(this, values[i].get()))
            assert(false && "cannot binop (fix this message)");
        this->append(values[i]);
    }
}

std::shared_ptr<Str>
Str::filter(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure.get());

    auto acc = std::make_shared<Str>();

    int i = 0;
    for (auto &c : m_value) {
        std::shared_ptr<Char> cx = nullptr;
        if (c == '\0')
            cx = std::dynamic_pointer_cast<Char>(m_chars.at(i)->copy());
        else {
            cx = std::make_shared<Char>(std::string(1, c));
            m_value.at(i) = '\0';
            m_chars.at(i) = cx;
        }
        std::vector<std::shared_ptr<Obj>> values = {cx};
        std::shared_ptr<Obj> filter_result = cl->call(values, ctx);
        if (dynamic_cast<Bool *>(filter_result.get())->boolean())
            acc->append(cx);
        ++i;
    }

    return acc;
}

void
Str::foreach(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure.get());
    for (size_t i = 0; i < m_value.size(); ++i) {
        std::shared_ptr<Char> cx = nullptr;
        if (m_value.at(i) == '\0')
            cx = std::dynamic_pointer_cast<Char>(m_chars.at(i)->copy());
        else {
            cx = std::make_shared<Char>(std::string(1, m_value.at(i)));
            m_value.at(i) = '\0';
            m_chars.at(i) = cx;
        }

        std::vector<std::shared_ptr<Obj>> values = {cx};
        cl->call(values, ctx);
    }
}

void
Str::trim(void) {
    UNIMPLEMENTED("Str::trim");
}

Type
Str::type(void) const {
    return Type::Str;
}

std::shared_ptr<Obj>
Str::binop(Token *op, std::shared_ptr<Obj> &other) {
    ASSERT_BINOP_COMPAT(this, other.get(), op);
    switch (op->type()) {
    case TokenType::Plus: {
        if (other->type() == Type::Char)
            return std::make_shared<Str>(this->value() + std::string(1, dynamic_cast<Char *>(other.get())->value()));
        return std::make_shared<Str>(this->value() + dynamic_cast<Str *>(other.get())->value());
    } break;
    case TokenType::Double_Equals: {
        if (other->type() == Type::Char)
            return std::make_shared<Bool>(this->value() == std::string(1, dynamic_cast<Char *>(other.get())->value()));
        return std::make_shared<Bool>(this->value() == dynamic_cast<Str *>(other.get())->value());
    } break;
    case TokenType::Bang_Equals: {
        if (other->type() == Type::Char)
            return std::make_shared<Bool>(this->value() != std::string(1, dynamic_cast<Char *>(other.get())->value()));
        return std::make_shared<Bool>(this->value() != dynamic_cast<Str *>(other.get())->value());
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
    return true;
}

std::vector<std::shared_ptr<Char>>
Str::value_as_earlchar(void) {
    std::vector<std::shared_ptr<Char>> values = {};
    std::for_each(m_value.begin(), m_value.end(), [&](char c){
        auto cx = std::make_shared<Char>(std::string(1, c));
        values.push_back(std::move(cx));
    });
    return values;
}

void
Str::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other.get(), stmt);
    if (other->type() == Type::Str) {
        Str *otherstr = dynamic_cast<Str *>(other.get());
        m_value = otherstr->value();
    }
    else {
        m_value.clear();
        m_value.push_back(dynamic_cast<Char *>(other.get())->value());
    }
}

std::shared_ptr<Obj>
Str::copy(void) {
    auto copy = std::make_shared<Str>();
    copy->m_value = m_value;
    copy->m_chars = m_chars;
    return copy;
}

bool
Str::eq(std::shared_ptr<Obj> &other) {
    if (other->type() != Type::Str)
        return false;
    return this->value() == dynamic_cast<Str *>(other.get())->value();
}

std::string
Str::to_cxxstring(void) {
    return this->value();
}

void
Str::spec_mutate(Token *op, const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    // std::vector<std::shared_ptr<Char>> prev = {};
    // std::for_each(m_value.begin(), m_value.end(), [&](std::shared_ptr<Char> k) {prev.push_back(k);});
    std::string prev = m_value;
    this->mutate(other, stmt); // does type checking
    switch (op->type()) {
    case TokenType::Plus_Equals: {
        for (int i = prev.size()-1; i >= 0; --i)
            m_value.insert(m_value.begin(), prev.at(i));
    } break;
    default: {
        Err::err_wtok(op);
        std::string msg = "invalid operator for special mutation `"+op->lexeme()+"` on str type";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<Obj>
Str::unaryop(Token *op) {
    (void)op;
    Err::err_wtok(op);
    std::string msg = "invalid unary operator on str type";
    throw InterpreterException(msg);
    return nullptr; // unreachable
}
