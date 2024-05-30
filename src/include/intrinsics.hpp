#ifndef INTRINSICS_H
#define INTRINSICS_H

#include <functional>
#include <unordered_map>

#include "interpreter.hpp"
#include "ctx.hpp"
#include "ast.hpp"

namespace Intrinsics {
  bool is_intrinsic_function(const std::string &id);
  Interpreter::ExprEvalResult run_intrinsic_function(Ctx &ctx, ExprFuncCall *expr);

  // Intrinsic functions
  Interpreter::ExprEvalResult print(Ctx &ctx, ExprFuncCall *expr);

  using IntrinsicFunction = Interpreter::ExprEvalResult(*)(Ctx&, ExprFuncCall*);

  extern const std::unordered_map<std::string, IntrinsicFunction> intrinsic_functions;
};

#endif // INTRINSICS_H
