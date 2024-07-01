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

Str::Str(std::string value) {
    std::for_each(value.begin(), value.end(), [&](char c) {
        m_value.push_back(new Char(std::string(1, c)));
    });
}

std::string Str::value(void) {
    std::string value = "";
    std::for_each(m_value.begin(), m_value.end(), [&](auto &c){value += c->value();});
    return value;
}

Char *Str::nth(Obj *idx) {
    auto *index = dynamic_cast<Int *>(idx);
    if (index->value() < 0 || static_cast<size_t>(index->value()) > this->value().size()) {
        ERR_WARGS(Err::Type::Fatal, "index %d is out of range of length %zu",
                  index->value(), this->value().size());
    }
    return m_value[index->value()];
}

List *Str::split(Obj *delim) {
    assert(delim->type() == Type::Str);

    std::vector<Obj *> splits;
    std::string delim_str = dynamic_cast<Str *>(delim)->value();
    std::string::size_type start = 0;

    auto pos = this->value().find(delim_str);

    while (pos != std::string::npos) {
        splits.push_back(new Str(this->value().substr(start, pos-start)));
        start = pos+delim_str.length();
        pos = this->value().find(delim_str, start);
    }
    splits.push_back(new Str(this->value().substr(start)));

    return new List(std::move(splits));
}

Str *Str::remove_lines(void) {
    for (size_t i = 0; i < m_value.size(); ++i) {
        if (m_value[i]->value() == '\n') {
            delete m_value[i];
            m_value.erase(m_value.begin()+i);
            --i;
        }
    }
    return dynamic_cast<Str *>(this->copy());
}

Str *Str::substr(Int *idx1, Int *idx2) {
    std::string sub = this->value().substr(idx1->value(), idx2->value());
    return new Str(sub);
}

Obj *Str::pop(Obj *idx) {
    assert(idx->type() == earl::value::Type::Int);
    earl::value::Int *index = dynamic_cast<earl::value::Int *>(idx);
    m_value.erase(m_value.begin() + index->value());
    return new Void();
}

Type Str::type(void) const {
    return Type::Str;
}

Obj *Str::binop(Token *op, Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot binop (fix this message)");
    }

    Obj *tmp = other;
    // FIXME
    // if (other->type() == Type::None) {
    //     auto *none = dynamic_cast<None *>(tmp);
    //     assert(none->value());
    //     tmp = none->value();
    // }

    switch (op->type()) {
    case TokenType::Plus: {
        return new Str(this->value() + dynamic_cast<Str *>(tmp)->value());
    } break;
    case TokenType::Double_Equals: {
        return new Bool(static_cast<int>(this->value() == dynamic_cast<Str *>(tmp)->value()));
    } break;
    default: {
        Err::err_wtok(op);
        ERR(Err::Type::Fatal, "invalid binary operator");
    }
    }
}

bool Str::boolean(void) {
    return true;
}

std::vector<Char *> &Str::value_raw(void) {
    return m_value;
}

void Str::mutate(Obj *other) {
    assert(other->type() == Type::Str);

    Str *otherstr = dynamic_cast<Str *>(other);

    std::for_each(m_value.begin(), m_value.end(), [](auto &this_c) {delete this_c;});
    m_value.clear();

    std::for_each(otherstr->value_raw().begin(), otherstr->value_raw().end(), [&](earl::value::Char *other_c) {
        m_value.push_back(dynamic_cast<earl::value::Char *>(other_c->copy()));
    });
}

Obj *Str::copy(void) {
    return new Str(this->value());
}

bool Str::eq(Obj *other) {
    if (other->type() != Type::Str)
        return false;
    return this->value() == dynamic_cast<Str *>(other)->value();
}
