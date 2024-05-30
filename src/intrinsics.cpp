#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

#include "intrinsics.hpp"
#include "interpreter.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"

using IntrinsicFunction = Interpreter::ExprEvalResult(*)(Ctx&, ExprFuncCall*);

const std::unordered_map<std::string, IntrinsicFunction> intrinsic_functions = {
  {"print", Intrinsics::print},
};

bool Intrinsics::is_intrinsic_function(const std::string &id) {
  return std::find(Intrinsics::intrinsic_funcs.begin(),
                   Intrinsics::intrinsic_funcs.end(), id)
    != Intrinsics::intrinsic_funcs.end();
}

Interpreter::ExprEvalResult Intrinsics::run_intrinsic_function(Ctx &ctx, ExprFuncCall *expr) {
  
}

Interpreter::ExprEvalResult Intrinsics::print(Ctx &ctx, ExprFuncCall *expr) {
  for (size_t i = 0; i < expr->m_params.size(); ++i) {
    std::unique_ptr<Expr> &e = expr->m_params[i];
    Interpreter::ExprEvalResult param = Interpreter::eval_expr(e.get(), ctx);

    if (param.m_expr_term_type == ExprTermType::Int_Literal) {
      std::cout << std::any_cast<int>(param.m_expr_value) <<
        (i == expr->m_params.size()-1 ? "" : " ");
    }

    else if (param.m_expr_term_type == ExprTermType::Str_Literal) {
      assert(false && "unimplemented");
    }

    // Identifier
    else {
      Token *tok = std::any_cast<Token *>(param.m_expr_value);
      EarlVar &var = ctx.get_earlvar_from_scope(tok->lexeme());
      switch (var.m_type) {
      case EarlTy::Type::Int: {
        std::cout << std::any_cast<int>(var.m_value) << (i == expr->m_params.size()-1 ? "" : " ");
      } break;
      default:
        ERR_WARGS(ErrType::Fatal, "invalid type for printing (%d)", (int)var.m_type);
      }
    }
  }
  std::cout << '\n';
}

