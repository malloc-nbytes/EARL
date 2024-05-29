#ifndef CTX_H
#define CTX_H

#include <any>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"

class Ctx {
  std::vector<std::unordered_map<std::string, EarlVar>> m_scope;
  std::unordered_map<EarlTy, std::vector<EarlTy>> m_earl_compat_tys;

public:
  Ctx();
  ~Ctx() = default;

  void pop_scope(void);
  void push_scope(void);

  bool earlvar_in_scope(const std::string &id);
  void add_earlvar_to_scope(std::unique_ptr<Token> id, EarlTy type, bool allocd, std::any value = nullptr);
};

#endif // CTX_H
