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

#include <cassert>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Slice::Slice(std::shared_ptr<Obj> start, std::shared_ptr<Obj> end)
    : m_start(start), m_end(end) {}

std::shared_ptr<Obj> &
Slice::start(void) {
    return m_start;
}

std::shared_ptr<Obj> &
Slice::end(void) {
    return m_end;
}

/*** OVERRIDES ***/
Type
Slice::type(void) const {
    return Type::Slice;
}

void
Slice::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other.get(), stmt);
    ASSERT_CONSTNESS(this, stmt);
    auto othersl = dynamic_cast<Slice *>(other.get());
    m_start = othersl->start();
    m_end = othersl->end();
}

std::shared_ptr<Obj>
Slice::copy(void) {
    return std::make_shared<Slice>(m_start, m_end);
}

bool
Slice::eq(Obj *other) {
    if (other->type() != Type::Slice)
        return false;
    auto __other = dynamic_cast<Slice *>(other);
    return m_start->eq(__other->start().get()) && m_end->eq(__other->end().get());
}

std::string
Slice::to_cxxstring(void) {
    std::string res = "<Slice { ";
    auto &start = this->start();
    auto &end = this->end();
    res += start->to_cxxstring();
    res += " : ";
    res += end->to_cxxstring();
    res += " }> ";
    return res;
}
