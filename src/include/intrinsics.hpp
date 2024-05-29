#ifndef INTRINSICS_H
#define INTRINSICS_H

#include <unordered_map>

#include "interpreter.hpp"
#include "ctx.hpp"
#include "ast.hpp"

namespace Intrinsics {
  extern const std::vector<std::string> intrinsics;

  void print(Ctx &ctx, ExprFuncCall *expr);
};


#endif // INTRINSICS_H
