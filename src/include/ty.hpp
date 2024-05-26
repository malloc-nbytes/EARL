#ifndef TY_H
#define TY_H

#include <unordered_map>

enum class EarlTy {
  Int,
  Str,
};

std::unordered_map<EarlTy, EarlTy> earl_compat_tys = { 
  {EarlTy::Int, EarlTy::Int},
  {EarlTy::Str, EarlTy::Str},
};

#endif // TY_H
