#include <cassert>

#include "earl.hpp"
#include "err.hpp"
#include "common.hpp"

using namespace earl::function;

Obj::Obj(StmtDef *stmtdef) : m_stmtdef(stmtdef) {
    m_local.emplace_back();
}

const std::string &Obj::id(void) const {
    return m_stmtdef->m_id->lexeme();
}

StmtBlock *Obj::block(void) const {
    return m_stmtdef->m_block.get();
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
        const std::string &id = m_stmtdef->m_args[i].first->lexeme();
        const std::string &type = m_stmtdef->m_args[i].second->lexeme();
        earl::value::Obj *value = values[i];

        if (!earl::value::type_is_compatable(value, earl::value::of_str(type))) {
            ERR_WARGS(Err::Type::Fatal,
                      "type mismatch in function `%s` parameter `%s`",
                      id.c_str(),
                      m_stmtdef->m_id->lexeme().c_str());
        }

        // NOTE: To handle references, replace:
        //     std::unique_ptr<earl::value::Obj>(value->copy())
        // with:
        //     std::unique_ptr<earl::value::Obj>(value)
        earl::variable::Obj *var =
            new earl::variable::Obj(m_stmtdef->m_args[i].first.get(),
                                    std::unique_ptr<earl::value::Obj>(value->copy()));

        add_local(var);
    }
}

bool Obj::is_world(void) const {
    assert(m_stmtdef);
    return (m_stmtdef->m_attrs & static_cast<uint32_t>(FuncAttr::World)) != 0;
}

