#include <cassert>
#include <iostream>
#include <unordered_map>

#include "intrinsics.hpp"
#include "interpreter.hpp"
#include "ctx.hpp"
#include "ast.hpp"

const std::vector<std::string> Intrinsics::intrinsics = {
  "print",
};

void Intrinsics::print(Ctx &ctx, ExprFuncCall *expr) {
   for (std::unique_ptr<Expr> &e : expr->m_params) {
      ExprEvalResult param = eval_expr(e.get(), ctx);
      if (param.m_expr_term_type == ExprTermType::Int_Literal) {
        std::cout << std::any_cast<int>(param.m_expr_value) << '\n';
      }
    }
}
