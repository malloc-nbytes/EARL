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

#include "earl.hpp"
#include "err.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "ctx.hpp"

using namespace earl::value;

Enum::Enum(StmtEnum *stmt, std::unordered_map<std::string, std::shared_ptr<variable::Obj>> elems)
    : m_stmt(stmt), m_elems(std::move(elems)) {
    m_id = stmt->m_id.get();
}

const std::string &
Enum::id(void) const {
    return m_stmt->m_id->lexeme();
}

std::shared_ptr<earl::variable::Obj>
Enum::get_entry(const std::string &id) {
    auto it = m_elems.find(id);
    assert(it != m_elems.end());
    return it->second;
}

bool
Enum::has_entry(const std::string &id) const {
    return m_elems.find(id) != m_elems.end();
}

/*** OVERRIDES ***/
Type
Enum::type(void) const  {
    return Type::Enum;
}

std::shared_ptr<Obj>
Enum::binop(Token *op, std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Enum::binop");
}

bool
Enum::boolean(void) {
    UNIMPLEMENTED("Enum::boolean");
}

void
Enum::mutate(const std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Enum::mutate");
}

std::shared_ptr<Obj>
Enum::copy(void) {
    UNIMPLEMENTED("Enum::copy");
}

bool
Enum::eq(std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Enum::eq");
}

std::string
Enum::to_cxxstring(void) {
    UNIMPLEMENTED("Enum::to_cxxstring");
}

