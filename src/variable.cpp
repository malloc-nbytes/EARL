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

#include "earl.hpp"
#include "utils.hpp"
#include "common.hpp"

using namespace earl::variable;

Obj::Obj(Token *id, std::shared_ptr<earl::value::Obj> value, uint32_t attrs, std::string info)
    : m_id(id), m_value(value), m_attrs(attrs), m_info(info) {
    m_constness = (attrs & static_cast<uint32_t>(Attr::Const)) != 0 ? true : false;
}

std::string
Obj::get_info(void) {
    return m_info;
}

Token *
Obj::gettok(void) {
    return m_id;
}

const std::string &Obj::id(void) const {
    return m_id->lexeme();
}

std::shared_ptr<earl::value::Obj> Obj::value(void) const {
    return m_value;
}

bool Obj::is_ref(void) const {
    return (m_attrs & static_cast<uint32_t>(Attr::Ref)) != 0;
}

bool Obj::is_pub(void) const {
    return (m_attrs & static_cast<uint32_t>(Attr::Pub)) != 0;
}

std::shared_ptr<Obj> Obj::copy(void) {
    return std::make_shared<Obj>(m_id, m_value->copy(), m_attrs);
}

earl::value::Type Obj::type(void) const {
    return m_value->type();
}

void
Obj::reset(std::shared_ptr<earl::value::Obj> value) {
    m_value = value;
}
