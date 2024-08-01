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

std::vector<std::shared_ptr<Obj>> &List::value(void) {
    return m_value;
}

Type List::type(void) const {
    return Type::List;
}

std::shared_ptr<Obj> List::nth(std::shared_ptr<Obj> &idx) {
    (void)idx;
    UNIMPLEMENTED("List::nth");
}

std::shared_ptr<Obj> List::rev(void) {
    UNIMPLEMENTED("List::rev");
}

void List::pop(std::shared_ptr<Obj> &idx) {
    (void)idx;
    UNIMPLEMENTED("List::pop");
}

void List::append(std::vector<std::shared_ptr<Obj>> values) {
    for (size_t i = 0; i < values.size(); ++i) {
        m_value.push_back(values[i]->copy());
    }
}

std::shared_ptr<List> List::filter(std::shared_ptr<Closure> &closure, Ctx &ctx) {
    (void)closure;
    (void)ctx;
    UNIMPLEMENTED("List::filter");
}

void List::foreach(std::shared_ptr<Closure> &closure, Ctx &ctx) {
    (void)closure;
    (void)ctx;
    UNIMPLEMENTED("List::foreach");
}

std::shared_ptr<Obj> List::back(void) {
    UNIMPLEMENTED("List::back");
}

std::shared_ptr<Obj> List::binop(Token *op, std::shared_ptr<Obj> &other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("List::binop");
}

bool List::boolean(void) {
    return m_value.size() > 0;
}

void List::mutate(const std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("List::mutate");
}

std::shared_ptr<Obj> List::copy(void) {
    auto list = std::make_shared<List>();
    list->append(this->value());
    return list;
}

bool List::eq(std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("List::eq");
}

std::string List::to_cxxstring(void) {
    std::string res = "";
    for (auto &el : m_value)
        res += el->to_cxxstring();
    return res;
}
