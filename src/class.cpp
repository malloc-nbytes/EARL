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

Class::Class(StmtClass *stmtclass) : m_stmtclass(stmtclass) {}

const std::string &Class::id(void) const {
    return m_stmtclass->m_id->lexeme();
}

// void Obj::load_parameters(std::vector<earl::value::Obj *> values) {
//     for (size_t i = 0; i < values.size(); i++) {
//         earl::value::Obj *value = values[i];
//
//         earl::variable::Obj *var = nullptr;
//         if ((m_params[i].second & static_cast<uint32_t>(Attr::Ref)) != 0) {
//             var
//                 = new earl::variable::Obj(m_params[i].first.get(),
//                                           std::unique_ptr<earl::value::Obj>(value));
//         }
//         else {
//             var
//                 = new earl::variable::Obj(m_params[i].first.get(),
//                                           std::unique_ptr<earl::value::Obj>(value->copy()));
//         }
//
//         add_local(var);
//     }
// }

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
    UNIMPLEMENTED("Class::add_member");
}

Type Class::type(void) const {
    UNIMPLEMENTED("Class::type");
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


