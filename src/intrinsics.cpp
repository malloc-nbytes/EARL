#include <cassert>
#include <iostream>
#include <unordered_map>

#include "intrinsics.hpp"
#include "interpreter.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"

const std::vector<std::string> Intrinsics::intrinsic_funcs = {
  "print",
};

void Intrinsics::print(Ctx &ctx, ExprFuncCall *expr) {
  for (std::unique_ptr<Expr> &e : expr->m_params) {
    Interpreter::ExprEvalResult param = Interpreter::eval_expr(e.get(), ctx);
    if (param.m_expr_term_type == ExprTermType::Int_Literal) {
      std::cout << std::any_cast<int>(param.m_expr_value) << '\n';
    }
    else if (param.m_expr_term_type == ExprTermType::Str_Literal) {
      assert(false && "unimplemented");
    }
    else {
      // Identifier
      Token *tok = std::any_cast<Token *>(param.m_expr_value);
      EarlVar &var = ctx.get_earlvar_from_scope(tok->lexeme());
      // std::cout << var.m_value.type().name() << std::endl;
      switch (var.m_type) {
      case EarlTy::Type::Int: {
        std::cout << std::any_cast<int>(var.m_value) << std::endl;
      } break;
      default:
        ERR_WARGS(ErrType::Fatal, "invalid type for printing (%d)", (int)var.m_type);
      }
    }
  }
}
