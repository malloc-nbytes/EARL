#include <unordered_map>

#include "earlty.hpp"
#include "common.hpp"
#include "err.hpp"

const std::unordered_map<std::string, EarlTy::Type> EarlTy::typemap = {
  {COMMON_EARLTY_INT32, EarlTy::Type::Int},
  {COMMON_EARLTY_STR, EarlTy::Type::Str},
};

const std::unordered_map<EarlTy::Type, std::vector<EarlTy::Type>> EarlTy::m_earl_compat_tys = {
  {EarlTy::Type::Int, {EarlTy::Type::Int}},
  {EarlTy::Type::Str, {EarlTy::Type::Str}},
};

EarlTy::Type EarlTy::of_str(const std::string &s) {
  auto it = typemap.find(s);
  if (it != typemap.end()) {
    return it->second;
  } else {
    ERR_WARGS(Fatal, "EarlTy::of_str: type `%s` is not a valid EARL type", s.c_str());
  }
}

bool EarlTy::earlvar_type_compat(EarlTy::Type ty1, EarlTy::Type ty2) {
  auto entry = EarlTy::m_earl_compat_tys.find(ty1);
  if (entry == EarlTy::m_earl_compat_tys.end()) {
    ERR_WARGS(ErrType::Fatal, "the type %d is not a valid EARL type",
              static_cast<int>(ty1));
  }
  for (auto compat : entry->second) {
    if (compat == ty2) {
      return true;
    }
  }
  return false;
}
