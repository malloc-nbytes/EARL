#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

#include "intrinsics.hpp"
#include "interpreter.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"
#include "utils.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicFunction> Intrinsics::intrinsic_functions = {
    {"print", &Intrinsics::intrinsic_print}
};

earl::value::Obj *Intrinsics::call(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    const std::string &id = expr->m_id->lexeme();
    return Intrinsics::intrinsic_functions.at(id)(expr, params, ctx);
}

bool Intrinsics::is_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_functions.find(id) != Intrinsics::intrinsic_functions.end();
}

earl::value::Obj *Intrinsics::intrinsic_print(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    for (size_t i = 0; i < params.size(); ++i) {
        earl::value::Obj *param = params.at(i);
        switch (param->type()) {
        case earl::value::Type::Int: {
            earl::value::Int *intparam = dynamic_cast<earl::value::Int *>(param);
            std::cout << intparam->value();
        } break;
        case earl::value::Type::Str: {
            earl::value::Str *strparam = dynamic_cast<earl::value::Str *>(param);
            std::cout << strparam->value();
        } break;
        default: {
            ERR_WARGS(Err::Type::Fatal, "intrinsic_print: unknown parameter type %d", static_cast<int>(param->type()));
        } break;
        }
    }
    std::cout << '\n';
    return nullptr;
}
