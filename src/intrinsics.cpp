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
    {"assert", &Intrinsics::intrinsic_assert},
    {"len", &Intrinsics::intrinsic_len},
};

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> Intrinsics::intrinsic_member_functions = {
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
};

earl::value::Obj *Intrinsics::call(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    const std::string &id = expr->m_id->lexeme();
    return Intrinsics::intrinsic_functions.at(id)(expr, params, ctx);
}

bool Intrinsics::is_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_functions.find(id) != Intrinsics::intrinsic_functions.end();
}

bool Intrinsics::is_member_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_member_functions.find(id) != Intrinsics::intrinsic_member_functions.end();
}

earl::value::Obj *Intrinsics::call_member(const std::string &id, earl::value::Obj *accessor, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    return Intrinsics::intrinsic_member_functions.at(id)(accessor, params, ctx);
}

earl::value::Obj *Intrinsics::intrinsic_member_nth(earl::value::Obj *obj, std::vector<earl::value::Obj *> &idx, Ctx &ctx) {
    (void)ctx;
    earl::value::List *list = dynamic_cast<earl::value::List *>(obj);
    assert(idx.size() == 1);
    return list->nth(idx[0]);
}

earl::value::Obj *Intrinsics::intrinsic_member_rev(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;
    assert(unused.size() == 0);
    assert(obj->type() == earl::value::Type::List);

    auto *list = dynamic_cast<earl::value::List *>(obj);
    list->rev();

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_append(earl::value::Obj *obj, std::vector<earl::value::Obj *> &values, Ctx &ctx) {
    (void)ctx;
    assert(obj->type() == earl::value::Type::List);
    auto *lst = dynamic_cast<earl::value::List *>(obj);
    lst->append(values);
    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_len(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)expr;
    (void)ctx;
    assert(params.size() == 1);
    if (params[0]->type() == earl::value::Type::List) {
        return new earl::value::Int(dynamic_cast<earl::value::List *>(params[0])->value().size());
    }
    else if (params[0]->type() == earl::value::Type::Str) {
        return new earl::value::Int(dynamic_cast<earl::value::Str *>(params[0])->value().size());
    }
    ERR_WARGS(Err::Type::Fatal, "canot call `len` on unsupported type (%d)", (int)params[0]->type());
    return nullptr;
}

earl::value::Obj *Intrinsics::intrinsic_assert(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;

    for (size_t i = 0; i < params.size(); ++i) {
        earl::value::Obj *param = params.at(i);
        if (!param->boolean()) {
            Err::err_wtok(expr->m_id.get());
            token_dump_until_eol(expr->m_id.get());
            ERR_WARGS(Err::Type::Assertion,
                      "assertion failure (expression=%zu) (earl::value::Type=%d)",
                      i+1, (int)param->type());
        }
    }

    return new earl::value::Void();
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
    return new earl::value::Void();
}
