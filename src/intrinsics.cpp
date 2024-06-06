#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

#include "intrinsics.hpp"
#include "interpreter.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicFunction> Intrinsics::intrinsic_functions = {
    {"print", &Intrinsics::print},
};

bool Intrinsics::is_intrinsic_function(const std::string &id) {
    return intrinsic_functions.find(id) != intrinsic_functions.end();
}

Interpreter::ExprEvalResult Intrinsics::run_intrinsic_function(ExprFuncCall *expr, std::vector<Interpreter::ExprEvalResult> params, Ctx &ctx) {
    auto retval = Intrinsics::intrinsic_functions.at(expr->m_id->lexeme())(expr, params, ctx);

    return Interpreter::ExprEvalResult{};
}

Interpreter::ExprEvalResult Intrinsics::print(ExprFuncCall *expr, std::vector<Interpreter::ExprEvalResult> params, Ctx &ctx) {
    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        Interpreter::ExprEvalResult param = params[i];

        if (param.m_earl_type == EarlTy::Type::Int) {
            std::cout << std::any_cast<int>(param.value());
        }

        else if (param.m_earl_type == EarlTy::Type::Str) {
            std::cout << std::any_cast<std::string>(param.value());
        }
        else {
            ERR_WARGS(ErrType::Fatal, "no printing for earltype %d", static_cast<int>(param.m_earl_type));
        }
    }
    std::cout << '\n';
    return Interpreter::ExprEvalResult{};
}

