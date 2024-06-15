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
    {"print", &Intrinsics::intrinsic_print},
    {"assert", &Intrinsics::intrinsic_assert}
};

earl::value::Obj *Intrinsics::call(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    const std::string &id = expr->m_id->lexeme();
    return Intrinsics::intrinsic_functions.at(id)(expr, params, ctx);
}

bool Intrinsics::is_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_functions.find(id) != Intrinsics::intrinsic_functions.end();
}

earl::value::Obj *Intrinsics::intrinsic_assert(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;

    for (size_t i = 0; i < params.size(); ++i) {
        earl::value::Obj *param = params.at(i);
        if (!param->boolean()) {
            Err::err_wtok(expr->m_id.get());
            ERR(Err::Type::Assertion, "assertion failure");
        }
    }

    return nullptr; // TODO: change from null
}

earl::value::Obj *Intrinsics::intrinsic_print(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;

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
        case earl::value::Type::List: {
            earl::value::List *listparam = dynamic_cast<earl::value::List *>(param);
            std::cout << '[';
            std::vector<earl::value::Obj *> &list = listparam->value();
            for (size_t j = 0; j < list.size(); ++j) {
                earl::value::Obj *listitem = list.at(j);
                switch (listitem->type()) {
                case earl::value::Type::Int: {
                    earl::value::Int *intitem = dynamic_cast<earl::value::Int *>(listitem);
                    std::cout << intitem->value();
                } break;
                case earl::value::Type::Str: {
                    earl::value::Str *stritem = dynamic_cast<earl::value::Str *>(listitem);
                    std::cout << stritem->value();
                } break;
                default: {
                    Err::err_wtok(expr->m_id.get());
                    ERR_WARGS(Err::Type::Fatal, "intrinsic_print: unknown list item type %d", static_cast<int>(listitem->type()));
                } break;
                }
                if (j != list.size() - 1) {
                    std::cout << ", ";
                }
            }

            std::cout << ']';
        } break;
        default: {
            Err::err_wtok(expr->m_id.get());
            ERR_WARGS(Err::Type::Fatal, "intrinsic_print: unknown parameter type %d", static_cast<int>(param->type()));
        } break;
        }
    }
    std::cout << '\n';
    return nullptr; // TODO: change from null
}
