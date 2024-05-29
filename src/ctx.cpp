#include <any>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"
#include "ctx.hpp"

Ctx::Ctx() {
  m_scope.emplace_back();

  m_earl_compat_tys[EarlTy::Int] = {EarlTy::Int};
  m_earl_compat_tys[EarlTy::Str] = {EarlTy::Str};
}

void Ctx::pop_scope(void) {
  m_scope.pop_back();
}

void Ctx::push_scope(void) {
  m_scope.emplace_back();
}

void Ctx::add_earlvar_to_scope(std::unique_ptr<Token> id, EarlTy type, bool allocd, std::any value) {
  std::string name = id->lexeme();
  m_scope.back().emplace(name, EarlVar(std::move(id), type, allocd, std::move(value)));
}

bool Ctx::earlvar_in_scope(const std::string &id) {
  for (std::unordered_map<std::string, EarlVar> &map : m_scope) {
    if (map.find(id) != map.end()) {
      return true;
    }
  }
  return false;
}
