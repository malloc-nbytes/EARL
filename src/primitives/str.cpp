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
                ERR_WARGS(Err::Type::Fatal, "unkown escape sequence `%c%c`", '\\', c);
            }
            escape = false;
        }
        else {
            m_value.push_back(std::make_shared<Char>(std::string(1, c)));
        }
    }
}

std::string Str::value(void) {
    std::string value = "";
    std::for_each(m_value.begin(), m_value.end(), [&](auto &c){value += c->value();});
    return value;
}

std::shared_ptr<Char> Str::nth(std::shared_ptr<Obj> &idx) {
    if (idx->type() != Type::Int)
        ERR(Err::Type::Fatal, "invalid index when accessing value in a list");

    auto index = dynamic_cast<Int *>(idx.get());
    if (index->value() < 0 || static_cast<size_t>(index->value()) > this->value().size()) {
        ERR_WARGS(Err::Type::Fatal, "index %d is out of str range of length %zu",
                  index->value(), this->value().size());
    }

    return m_value[index->value()];
}

std::shared_ptr<List> Str::split(std::shared_ptr<Obj> &delim) {
    (void)delim;
    UNIMPLEMENTED("Str::split");
}

std::shared_ptr<Str> Str::substr(std::shared_ptr<Int> &idx1, std::shared_ptr<Int> &idx2) {
    (void)idx1;
    (void)idx2;
    UNIMPLEMENTED("Str::substr");
}

void Str::pop(std::shared_ptr<Obj> &idx) {
    (void)idx;
    UNIMPLEMENTED("Str::pop");
}

Type Str::type(void) const {
    return Type::Str;
}

std::shared_ptr<Obj> Str::binop(Token *op, std::shared_ptr<Obj> &other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("Str::binop");
}

bool Str::boolean(void) {
    return true;
}

std::vector<std::shared_ptr<Char>> &Str::value_raw(void) {
    UNIMPLEMENTED("Str::value_raw");
}

void Str::mutate(const std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("Str::mutate");
}

std::shared_ptr<Obj> Str::copy(void) {
    auto copy = std::make_shared<Str>(this->value());
    return copy;
}

bool Str::eq(std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("Str::eq");
}

std::string Str::to_cxxstring(void) {
    return this->value();
}
