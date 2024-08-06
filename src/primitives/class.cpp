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

Class::Class(StmtClass *stmtclass, std::shared_ptr<Ctx> owner) : m_stmtclass(stmtclass), m_ctx(owner) {}

void Class::load_class_members(std::vector<std::shared_ptr<Obj>> &args) {
    assert(false);
}

std::shared_ptr<Ctx> &Class::ctx(void) {
    return m_ctx;
}

bool Class::is_pub(void) const {
    return (m_stmtclass->m_attrs & static_cast<uint32_t>(Attr::Pub)) != 0;
}

/*** OVERRIDES ***/
Type Class::type(void) const {
    return Type::Class;
}

std::shared_ptr<Obj> Class::binop(Token *op, std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Class::binop");
}

bool Class::boolean(void) {
    UNIMPLEMENTED("Class::boolean");
}

void Class::mutate(const std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Class::mutate");
}

std::shared_ptr<Obj> Class::copy(void) {
    UNIMPLEMENTED("Class::copy");
}

bool Class::eq(std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Class::eq");
}

std::string Class::to_cxxstring(void) {
    UNIMPLEMENTED("Class::to_cxxstring");
}
