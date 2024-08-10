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

/*** OVERRIDES ***/
Type Tuple::type(void) const {
    return Type::Tuple;
}

std::shared_ptr<Obj> Tuple::binop(Token *op, std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Tuple::binop");
}
bool Tuple::boolean(void) {
    UNIMPLEMENTED("Tuple::boolean");
}

void Tuple::mutate(const std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Tuple::mutate");
}

std::shared_ptr<Obj> Tuple::copy(void) {
    std::vector<std::shared_ptr<Obj>> values = {};
    std::for_each(m_values.begin(), m_values.end(), [&](auto &v){values.push_back(v);});
    return std::make_shared<Tuple>(values);
}

bool Tuple::eq(std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Tuple::eq");
}

std::string Tuple::to_cxxstring(void) {
    UNIMPLEMENTED("Tuple::to_cxxstring");
}

void Tuple::spec_mutate(Token *op, const std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Tuple::spec_mutate");
}

std::shared_ptr<Obj> Tuple::unaryop(Token *op) {
    UNIMPLEMENTED("Tuple::unaryop");
}


