#ifndef EARLTY_H
#define EARLTY_H

#include <string>
#include <unordered_map>

namespace EarlTy {

  enum class Type {
    Int,
    Str,
  };

  extern const std::unordered_map<std::string, Type> typemap;

  Type of_str(const std::string &s);

};

#endif // EARLTY_H
