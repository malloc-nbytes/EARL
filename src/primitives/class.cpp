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

void
Class::load_class_members(std::vector<std::shared_ptr<Obj>> &args) {
    (void)args;
    assert(false);
}

std::shared_ptr<Ctx> &
Class::ctx(void) {
    return m_ctx;
}

bool
Class::is_experimental(void) const {
    return (m_stmtclass->m_attrs & static_cast<uint32_t>(Attr::Experimental)) != 0;
}

void
Class::disable_experimental_flag(void) {
    m_stmtclass->m_attrs &= ~static_cast<uint32_t>(Attr::Experimental);
}

bool
Class::is_pub(void) const {
    return (m_stmtclass->m_attrs & static_cast<uint32_t>(Attr::Pub)) != 0;
}

const std::string &
Class::id(void) const {
    return m_stmtclass->m_id->lexeme();
}

std::vector<std::shared_ptr<earl::variable::Obj>> &
Class::get_members(void) {
    return m_members;
}

/*** OVERRIDES ***/
Type
Class::type(void) const {
    return Type::Class;
}

void
Class::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);

    auto other0 = dynamic_cast<Class *>(other);
    m_stmtclass = other0->m_stmtclass;
    m_ctx = other0->m_ctx;
    m_members = other0->m_members;
    m_methods = other0->m_methods;
    m_member_assignees = other0->m_member_assignees;
}

std::shared_ptr<Obj>
Class::copy(void) {
    assert(m_ctx->type() == CtxType::Class);
    auto ctx_copy = dynamic_cast<ClassCtx *>(m_ctx.get())->deep_copy();
    auto class_copy = std::make_shared<Class>(m_stmtclass, ctx_copy);
    return class_copy;
}

std::string
Class::to_cxxstring(void) {
    std::string res = "<Class " + this->id() + " { ";

    auto class_ctx = dynamic_cast<ClassCtx *>(this->ctx().get());
    auto members = class_ctx->get_printable_members();

    for (size_t i = 0; i < members.size(); ++i) {
        auto &mem = members[i];
        res += mem->id() + " = ";
        res += mem->value()->to_cxxstring();
        if (i != members.size()-1)
            res += ", ";
    }

    res += " }>";
    return res;
}
