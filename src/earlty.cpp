#include <unordered_map>

#include "earlty.hpp"
#include "common.hpp"
#include "err.hpp"

const std::unordered_map<std::string, EarlTy::Type> EarlTy::typemap = {
  {COMMON_EARLTY_INT32, EarlTy::Type::Int},
  {COMMON_EARLTY_STR, EarlTy::Type::Str},
};

const std::unordered_map<EarlTy::Type, std::vector<EarlTy::Type>> m_earl_compat_tys = {
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
