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

Class::Class(StmtClass *stmtclass, Ctx *owner) : m_stmtclass(stmtclass), m_owner(owner) {}

const std::string &Class::id(void) const {
    return m_stmtclass->m_id->lexeme();
}

void Class::constructor(Ctx &ctx) {
    UNIMPLEMENTED("Class::constructor");
    // for (size_t i = 0; i < m_stmtclass->m_members.size(); ++i) {
    //     StmtLet *let = m_stmtclass->m_members[i].get();
    //     // auto *var = new earl::variable::Obj();
    // }
}

void Class::add_member_assignees(std::vector<Token *> &assignees) {
    std::for_each(assignees.begin(), assignees.end(), [&](auto &a) {
        m_member_assignees.push_back(a);
    });
}

void Class::add_member_assignee(Token *assignee) {
    m_member_assignees.push_back(assignee);
}

void Class::add_method(std::unique_ptr<function::Obj> func) {
    m_methods.push_back(std::move(func));
}

void Class::add_member(std::unique_ptr<variable::Obj> var) {
    m_members.push_back(std::move(var));
}

earl::function::Obj *Class::get_method(const std::string &id) {
    for (size_t i = 0; i < m_methods.size(); ++i) {
        if (id == m_methods[i]->id()) {
            return m_methods[i].get();
        }
    }

    // ERR_WARGS(Err::Type::Fatal, "no method named %s exists in class %s",
    //           id.c_str(), this->id().c_str());
    return nullptr; // unreachable
}

earl::variable::Obj *Class::get_member(const std::string &id) {
    for (auto &mem : m_members) {
        if (mem->id() == id) {
            return mem.get();
        }
    }
    return nullptr;
}

Type Class::type(void) const {
    return Type::Class;
}

Obj *Class::binop(Token *op, Obj *other) {
    UNIMPLEMENTED("Class::binop");
}

bool Class::boolean(void) {
    UNIMPLEMENTED("Class::boolean");
}

void Class::mutate(Obj *other) {
    UNIMPLEMENTED("Class::mutate");
}

Obj *Class::copy(void) {
    UNIMPLEMENTED("Class::copy");
}


