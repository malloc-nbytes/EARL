#ifndef EARLTY_H
#define EARLTY_H

#include <string>
#include <unordered_map>
#include <vector>

namespace EarlTy {

  enum class Type {
    Int,
    Str,
  };

  extern const std::unordered_map<std::string, Type> typemap;
  extern const std::unordered_map<EarlTy::Type, std::vector<EarlTy::Type>> m_earl_compat_tys;

  Type of_str(const std::string &s);
};

#endif // EARLTY_H
