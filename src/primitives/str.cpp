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

Str::Str(std::string value) {
    bool escape = false;
    for (size_t i = 0; i < value.size(); ++i) {
        char c = value[i];
        if (!escape && c == '\\') {
            escape = true;
            continue;
        }
        if (escape) {
            switch (c) {
            case 'n': {
                m_value.push_back(std::make_shared<Char>(std::string(1, '\n')));
            } break;
            case 't': {
                m_value.push_back(std::make_shared<Char>(std::string(1, '\t')));
            } break;
            case '"': {
                m_value.push_back(std::make_shared<Char>(std::string(1, '"')));
            } break;
            case '\\': {
                m_value.push_back(std::make_shared<Char>(std::string(1, '\\')));
            } break;
            default:
                std::string msg = "unkown escape sequence `\\"+std::to_string(c)+"`";
                throw InterpreterException(msg);
            }
            escape = false;
        }
        else {
            m_value.push_back(std::make_shared<Char>(std::string(1, c)));
        }
    }
}

Str::Str(std::vector<std::shared_ptr<Char>> chars) {
    std::for_each(chars.begin(), chars.end(), [&](auto &c){m_value.push_back(c);});
}

std::string
Str::value(void) {
    std::string value = "";
    std::for_each(m_value.begin(), m_value.end(), [&](auto &c){value += c->value();});
    return value;
}

std::shared_ptr<Char>
Str::nth(std::shared_ptr<Obj> &idx) {
    if (idx->type() != Type::Int) {
        std::string msg = "invalid index when accessing value in a list";
        throw InterpreterException(msg);
    }

    auto index = dynamic_cast<Int *>(idx.get());
    if (index->value() < 0 || static_cast<size_t>(index->value()) > this->value().size()) {
        std::string msg = "index "+std::to_string(index->value())+" is out of str range of length "+std::to_string(this->value().size());
        throw InterpreterException(msg);
    }
    return m_value[index->value()];
}

std::shared_ptr<List>
Str::split(std::shared_ptr<Obj> &delim) {
    assert(delim->type() == Type::Str);

    std::vector<std::shared_ptr<Obj>> splits = {};
    std::string delim_str = dynamic_cast<Str *>(delim.get())->value();
    std::string::size_type start = 0;

    auto pos = this->value().find(delim_str);

    while (pos != std::string::npos) {
        splits.push_back(std::make_shared<Str>(this->value().substr(start, pos-start)));
        start = pos+delim_str.length();
        pos = this->value().find(delim_str, start);
    }
    splits.push_back(std::make_shared<Str>(this->value().substr(start)));

    return std::make_shared<List>(std::move(splits));
}

std::shared_ptr<Str>
Str::substr(std::shared_ptr<Obj> &idx1, std::shared_ptr<Obj> &idx2) {
    assert(idx1->type() == Type::Int);
    assert(idx2->type() == Type::Int);
    std::string sub = this->value().substr(dynamic_cast<Int*>(idx1.get())->value(), dynamic_cast<Int*>(idx2.get())->value());
    return std::make_shared<Str>(sub);
}

void
Str::pop(std::shared_ptr<Obj> &idx) {
    auto *idx1 = dynamic_cast<earl::value::Int *>(idx.get());
    m_value.erase(m_value.begin() + idx1->value());
}

std::shared_ptr<Obj>
Str::back(void) {
    if (m_value.size() == 0)
        return std::make_shared<Option>();
    return m_value.back()->copy();
}

std::shared_ptr<Str>
Str::rev(void) {
    auto str = std::make_shared<Str>();
    for (int i = m_value.size()-1; i >= 0; --i)
        str->append(m_value[i]);
    return str;
}

void
Str::append(std::shared_ptr<Obj> c) {
    if (c->type() == Type::Char)
        m_value.push_back(std::dynamic_pointer_cast<Char>(c));
    else {
        auto s = dynamic_cast<Str *>(c.get());
        for (auto &cx : s->value_raw())
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

    for (auto &c : this->value_raw()) {
        std::vector<std::shared_ptr<Obj>> values = {c};
        std::shared_ptr<Obj> filter_result = cl->call(values, ctx);
        if (dynamic_cast<Bool *>(filter_result.get())->boolean())
            acc->append(values[0]);
    }

    return acc;
}

void
Str::foreach(std::shared_ptr<Obj> &closure, std::shared_ptr<Ctx> &ctx) {
    Closure *cl = dynamic_cast<Closure *>(closure.get());
    for (size_t i = 0; i < m_value.size(); ++i) {
        std::vector<std::shared_ptr<Obj>> values = {m_value[i]};
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
        if (other->type() == Type::Char) {
            return std::make_shared<Str>(this->value() + std::string(1, dynamic_cast<Char *>(other.get())->value()));
        }
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

std::vector<std::shared_ptr<Char>> &
Str::value_raw(void) {
    return m_value;
}

void
Str::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other.get(), stmt);
    if (other->type() == Type::Str) {
        Str *otherstr = dynamic_cast<Str *>(other.get());
        m_value.clear();
        for (size_t i = 0; i < otherstr->value_raw().size(); ++i)
            m_value.push_back(otherstr->value_raw()[i]);
    }
    else {
        auto otherchar = std::dynamic_pointer_cast<Char>(other);
        m_value.clear();
        m_value.push_back(otherchar);
    }
}

std::shared_ptr<Obj>
Str::copy(void) {
    auto copy = std::make_shared<Str>(this->value());
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
    std::vector<std::shared_ptr<Char>> prev = {};
    std::for_each(m_value.begin(), m_value.end(), [&](std::shared_ptr<Char> k) {prev.push_back(k);});
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
