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
#include <cassert>
#include <cmath>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

ClassRef::ClassRef(StmtClass *stmt) : m_stmt(stmt) {}

const std::vector<std::string> &
ClassRef::get_info(void) {
    return m_stmt->m_info;
}

StmtClass *
ClassRef::get_stmt(void) {
    return m_stmt;
}

// Implements
Type
ClassRef::type(void) const {
    return Type::ClassRef;
}

std::string
ClassRef::to_cxxstring(void) {
    return "<Class Reference { src = "+m_stmt->m_id->lexeme()+" }>";
}

std::shared_ptr<Obj>
ClassRef::copy(void) {
    return std::make_shared<earl::value::ClassRef>(m_stmt);
}
