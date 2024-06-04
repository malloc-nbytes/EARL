#include <iostream>

#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"
#include "earlfunc.hpp"
#include "err.hpp"

EarlFunc::Func::Func(Token *id,
                     EarlTy::Type rettype,
                     std::vector<EarlVar *> args,
                     StmtBlock *block)
    : m_id(id),
      m_rettype(rettype),
      m_args(std::move(args)),
      m_block(block),
      m_local_scope(1) {}

void EarlFunc::Func::push_scope(void) {
    m_local_scope.back().push();
}

void EarlFunc::Func::pop_scope(void) {
    m_local_scope.back().pop();
}

void EarlFunc::Func::new_scope_context(void) {
    m_local_scope.emplace_back();
}

bool EarlFunc::Func::contains_local_earlvar(const std::string &id) {
    return m_local_scope.back().contains(id);
}

EarlVar *EarlFunc::Func::get_local_earlvar(const std::string &id) {
    EarlVar **var = m_local_scope.back().get(id);
    if (!*var)
        ERR_WARGS(ErrType::Fatal, "variable `%s` is not in local scope", id.c_str());
    return *var;
}

size_t EarlFunc::Func::context_size(void) {
    return m_local_scope.size();
}