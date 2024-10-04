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
#include "interpreter.hpp"

using namespace earl::function;

Obj::Obj(StmtDef *stmtdef,
         std::vector<std::pair<std::pair<Token *, __Type *>, uint32_t>> params,
         Token *tok,
         std::optional<__Type *> explicit_type,
         std::vector<std::string> info)
    : m_stmtdef(stmtdef),
      m_params(std::move(params)),
      m_tok(tok),
      m_explicit_type(explicit_type),
      m_info(std::move(info)) {
    m_id = tok->lexeme();
}

bool
Obj::is_explicit_typed(void) const {
    return m_explicit_type.has_value();
}

__Type *
Obj::get_explicit_type(void) {
    return m_explicit_type.value();
}

StmtDef *
Obj::get_stmtdef(void) {
    return m_stmtdef;
}

Token *
Obj::gettok(void) {
    return m_tok;
}

const std::string &
Obj::id(void) const {
    // return m_stmtdef->m_id->lexeme();
    return m_id;
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
                     std::shared_ptr<FunctionCtx> &new_ctx,
                     std::shared_ptr<Ctx> &old_ctx) {
    for (size_t i = 0; i < values.size(); ++i) {
        auto value = values[i];
        Token *id = m_params.at(i).first.first;
        __Type *ty = m_params.at(i).first.second;

        if (ty) {
            Interpreter::typecheck(ty, value.get(), old_ctx);
        }

        std::shared_ptr<earl::variable::Obj> var = nullptr;
        if ((m_params.at(i).second & static_cast<uint32_t>(Attr::Ref)) != 0)
            var = std::make_shared<earl::variable::Obj>(id, value);
        else
            var = std::make_shared<earl::variable::Obj>(id, value->copy());
        if ((m_params.at(i).second & static_cast<uint32_t>(Attr::Const)) != 0)
            var->value()->set_const();
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

std::shared_ptr<Obj>
Obj::copy(void) {
    return std::make_shared<Obj>(m_stmtdef, m_params, m_tok, m_explicit_type, m_info);
}

bool
Obj::param_at_is_ref(size_t i) const {
    return (m_params.at(i).second & static_cast<uint32_t>(Attr::Ref)) != 0;
}

uint32_t
Obj::attrs(void) const {
    return m_stmtdef->m_attrs;
}

const std::vector<std::string> &
Obj::info(void) const {
    return m_info;
}

void
Obj::change_id(const std::string &newid) {
    m_id = newid;
}


