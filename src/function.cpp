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

using namespace earl::function;

Obj::Obj(StmtDef *stmtdef, std::vector<std::pair<std::unique_ptr<Token>, uint32_t>> params) : m_stmtdef(stmtdef), m_params(std::move(params)) {
    m_local.emplace_back();
}

// TODO: free memory from the local variables
// in the local scope.
void Obj::clear_locals(void) {
    m_local.clear();
    m_local.emplace_back();
}

const std::string &Obj::id(void) const {
    return m_stmtdef->m_id->lexeme();
}

StmtBlock *Obj::block(void) const {
    return m_stmtdef->m_block.get();
}

size_t Obj::context_size(void) const {
    return m_local.size();
}

bool Obj::has_local(const std::string &id) {
    return m_local.back().contains(id);
}

void earl::function::Obj::push_scope(void) {
    m_local.back().push();
}

void earl::function::Obj::pop_scope(void) {
    m_local.back().pop();
}

void earl::function::Obj::new_scope_context(void) {
    m_local.emplace_back();
}

void earl::function::Obj::drop_scope_context(void) {
    m_local.pop_back();
}

earl::variable::Obj *Obj::get_local(const std::string &id) {
    auto **var = m_local.back().get(id);

    if (!var) {
        ERR_WARGS(Err::Type::Fatal, "variable `%s` is not in local scope", id.c_str());
    }

    return *var;
}

void Obj::add_local(variable::Obj *var) {
    if (has_local(var->id())) {
        ERR_WARGS(Err::Type::Fatal,
                  "variable `%s` already exists in function `%s`",
                  var->id().c_str(),
                  m_stmtdef->m_id->lexeme().c_str());
    }

    m_local.back().add(var->id(), var);
}

void Obj::load_parameters(std::vector<earl::value::Obj *> values) {
    for (size_t i = 0; i < values.size(); i++) {
        earl::value::Obj *value = values[i];

        // NOTE: To handle references, replace:
        //     std::unique_ptr<earl::value::Obj>(value->copy())
        // with:
        //     std::unique_ptr<earl::value::Obj>(value)
        // earl::variable::Obj *var =
        //     new earl::variable::Obj(m_stmtdef->m_args[i].get(),
        //                             std::unique_ptr<earl::value::Obj>(value->copy()));

        earl::variable::Obj *var = nullptr;

        if ((m_params[i].second & static_cast<uint32_t>(Attr::Ref)) != 0) {
            var = new earl::variable::Obj(m_params[i].first.get(), std::unique_ptr<earl::value::Obj>(value));
        }
        else {
            var = new earl::variable::Obj(m_params[i].first.get(), std::unique_ptr<earl::value::Obj>(value->copy()));
        }

        add_local(var);
    }
}

bool Obj::is_world(void) const {
    assert(m_stmtdef);
    return (m_stmtdef->m_attrs & static_cast<uint32_t>(Attr::World)) != 0;
}

bool Obj::is_pub(void) const {
    assert(m_stmtdef);
    return (m_stmtdef->m_attrs & static_cast<uint32_t>(Attr::Pub)) != 0;
}

void Obj::debug_dump(void) const {
    int i = 1;
    for (auto const &v : m_local) {
        std::cout << "context level: " << i << std::endl;
        std::cout << "  ";
        v.debug_dump();
    }
}

