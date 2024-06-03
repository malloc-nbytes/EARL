#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"
#include "earlfunc.hpp"
#include "err.hpp"

EarlFunc::Func::Func(Token *id,
                   EarlTy::Type rettype,
                   std::vector<std::unique_ptr<EarlVar>> args,
                   StmtBlock *block)
    : m_id(id),
      m_rettype(rettype),
      m_args(std::move(args)),
      m_block(block),
      m_local_scope(1) {}

void EarlFunc::Func::add_new_local_scope_context(void) {
    m_local_scope.emplace_back();
}

void EarlFunc::Func::add_earlvar_to_local_scope(std::unique_ptr<EarlVar> var) {
    std::string id = var->m_id->lexeme();
    m_local_scope.back().add(id, std::move(var));
}

EarlVar *EarlFunc::Func::get_earlvar_from_local_scope(const std::string &id) {
    EarlVar *var = m_local_scope.back().get(id);
    if (!var) {
        ERR_WARGS(Err::Fatal, "variable `%s` is not in scope", id.c_str());
    }
    return var;
}

void EarlFunc::Func::push_scope(void) {
    m_local_scope.back().push();
}

void EarlFunc::Func::pop_scope(void) {
    m_local_scope.back().pop();
}

bool EarlFunc::Func::has_earlvar_in_local_scope(const std::string &id) {
    return m_local_scope.back().contains(id);
}
