#include <iostream>

#include "common.hpp"
#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"
#include "earlfunc.hpp"
#include "err.hpp"

EarlFunc::Func::Func(Token *id,
                     EarlTy::Type rettype,
                     std::vector<EarlVar *> args,
                     StmtBlock *block,
                     uint32_t attrs)
    : m_id(id),
      m_rettype(rettype),
      m_args(std::move(args)),
      m_block(block),
      m_attrs(attrs) {
    m_local_scope.emplace_back();
}

void EarlFunc::Func::push_scope(void) {
    m_local_scope.back().push();
}

void EarlFunc::Func::pop_scope(void) {
    m_local_scope.back().pop();
}

void EarlFunc::Func::new_scope_context(void) {
    m_local_scope.emplace_back();
}

void EarlFunc::Func::drop_scope_context(void) {
    m_local_scope.pop_back();
}

bool EarlFunc::Func::contains_local_earlvar(const std::string &id) {
    return m_local_scope.back().contains(id);
}

EarlVar *EarlFunc::Func::get_local_earlvar(const std::string &id) {
    EarlVar **var = m_local_scope.back().get(id);
    if (!var || !*var)
        ERR_WARGS(Err::Type::Fatal, "variable `%s` is not in local scope", id.c_str());
    return *var;
}

void EarlFunc::Func::add_local_earlvar(EarlVar *var) {
    const std::string &id = var->m_id->lexeme();
    m_local_scope.back().add(id, var);
}

size_t EarlFunc::Func::context_size(void) {
    return m_local_scope.size();
}

void EarlFunc::Func::remove_local_earlvar(EarlVar *var) {
    const std::string &id = var->m_id->lexeme();
    m_local_scope.back().remove(id);
    abort();
}

bool EarlFunc::Func::is_world(void) const {
    return (m_attrs & static_cast<uint32_t>(FuncAttr::World)) != 0;
}
