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
#include "common.hpp"
#include "utils.hpp"
#include "ctx.hpp"

using namespace earl::function;

Obj::Obj(StmtDef *stmtdef, std::vector<std::pair<Token *, uint32_t>> params)
    : m_stmtdef(stmtdef), m_params(std::move(params)) {}

const std::string &
Obj::id(void) const {
    return m_stmtdef->m_id->lexeme();
}

size_t
Obj::params_len(void) const {
    return m_params.size();
}

StmtBlock *
Obj::block(void) const {
    return m_stmtdef->m_block.get();
}

void
Obj::load_parameters(std::vector<std::shared_ptr<earl::value::Obj>> &values,
                     std::shared_ptr<FunctionCtx> &new_ctx) {
    for (size_t i = 0; i < values.size(); ++i) {
        auto value = values[i];
        Token *id = m_params.at(i).first;
        std::shared_ptr<earl::variable::Obj> var = nullptr;
        if ((m_params.at(i).second & static_cast<uint32_t>(Attr::Ref)) != 0)
            var = std::make_shared<earl::variable::Obj>(id, value);
        else
            var = std::make_shared<earl::variable::Obj>(id, value->copy());
        new_ctx->variable_add(var);
    }
}

bool
Obj::is_world(void) const {
    assert(m_stmtdef);
    return (m_stmtdef->m_attrs & static_cast<uint32_t>(Attr::World)) != 0;
}

bool
Obj::is_pub(void) const {
    assert(m_stmtdef);
    return (m_stmtdef->m_attrs & static_cast<uint32_t>(Attr::Pub)) != 0;
}

Obj *
Obj::copy(void) {
    UNIMPLEMENTED("Obj::copy (function)");
}

bool
Obj::param_at_is_ref(size_t i) const {
    return (m_params.at(i).second & static_cast<uint32_t>(Attr::Ref)) != 0;
}
