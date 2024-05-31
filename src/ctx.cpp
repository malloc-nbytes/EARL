#include <any>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"
#include "err.hpp"
#include "ctx.hpp"

Ctx::Ctx() {
  m_scope.emplace_back();
}

void Ctx::pop_scope(void) {
  m_scope.pop_back();
  m_functions.pop_back();
}

void Ctx::push_scope(void) {
  m_scope.emplace_back();
  m_functions.emplace_back();
}

void Ctx::create_and_add_earlvar_to_scope(std::unique_ptr<Token> id, EarlTy::Type type, bool allocd, std::any value) {
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

EarlVar &Ctx::get_earlvar_from_scope(const std::string &id) {
  for (std::unordered_map<std::string, EarlVar> &map : m_scope) {
    auto it = map.find(id);
    if (it != map.end()) {
      return it->second;
    }
  }
  ERR_WARGS(ErrType::Fatal, "%s is not in scope", id.c_str());
}

void Ctx::add_function_to_scope(std::unique_ptr<Token> id,
                                EarlTy::Type rettype,
                                std::vector<EarlVar> args,
                                std::unique_ptr<StmtBlock> block) {
  std::string name = id->lexeme();
  m_functions.back().emplace(name, EarlFunc(std::move(id), rettype, args, std::move(block)));
}
