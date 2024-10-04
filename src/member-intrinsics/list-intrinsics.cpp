/** @file */

// MIT License

// Copyright (c) 2023 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>
#include <unordered_map>

#include "intrinsics.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_list_member_functions = {
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"back", &Intrinsics::intrinsic_member_back},
    {"filter", &Intrinsics::intrinsic_member_filter},
    {"foreach", &Intrinsics::intrinsic_member_foreach},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
    {"pop", &Intrinsics::intrinsic_member_pop},
    {"contains", &Intrinsics::intrinsic_member_contains},
    {"map", &Intrinsics::intrinsic_member_map},
    {"fold", &Intrinsics::intrinsic_member_fold},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_nth(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &idx,
                                 std::shared_ptr<Ctx> &ctx,
                                 Expr *expr) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(idx, "nth", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(idx[0], earl::value::Type::Int, 1, "nth", expr);
    if (obj->type() == earl::value::Type::List) {
        earl::value::List *list = dynamic_cast<earl::value::List *>(obj.get());
        return list->nth(idx[0], nullptr);//CHANGEME
    }
    else if (obj->type() == earl::value::Type::Str) {
        earl::value::Str *str = dynamic_cast<earl::value::Str *>(obj.get());
        return str->nth(idx[0].get(), nullptr);//CHANGEME
    }
    else if (obj->type() == earl::value::Type::Tuple) {
        earl::value::Tuple *tuple = dynamic_cast<earl::value::Tuple *>(obj.get());
        return tuple->nth(idx[0].get(), nullptr);//CHANGEME
    }
    else {
        Err::err_wexpr(expr);
        std::string msg = "`nth` member intrinsic is only defined for `list` and `str` types";
        throw InterpreterException(msg);
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_back(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx,
                                  Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "back", expr);
    if (obj->type() == earl::value::Type::List)
        return dynamic_cast<earl::value::List *>(obj.get())->back();
    else if (obj->type() == earl::value::Type::Tuple)
        return dynamic_cast<earl::value::Tuple *>(obj.get())->back();
    else
        return dynamic_cast<earl::value::Str *>(obj.get())->back();
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_filter(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(closure, 1, "filter", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(closure[0], earl::value::Type::Closure, 1, "filter", expr);
    if (obj->type() == earl::value::Type::List)
        return dynamic_cast<earl::value::List *>(obj.get())->filter(closure.at(0).get(), ctx);
    else if (obj->type() == earl::value::Type::Tuple)
        return dynamic_cast<earl::value::Tuple *>(obj.get())->filter(closure.at(0).get(), ctx);
    else
        return dynamic_cast<earl::value::Str *>(obj.get())->filter(closure.at(0).get(), ctx);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_foreach(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(closure, 1, "foreach", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(closure[0], earl::value::Type::Closure, 1, "foreach", expr);
    if (obj->type() == earl::value::Type::List)
        dynamic_cast<earl::value::List *>(obj.get())->foreach(closure.at(0).get(), ctx);
    else if (obj->type() == earl::value::Type::Tuple)
        dynamic_cast<earl::value::Tuple *>(obj.get())->foreach(closure.at(0).get(), ctx);
    else
        dynamic_cast<earl::value::Str *>(obj.get())->foreach(closure.at(0).get(), ctx);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_rev(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                 std::shared_ptr<Ctx> &ctx,
                                 Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "rev", expr);
    if (obj->type() == earl::value::Type::List)
        return dynamic_cast<earl::value::List *>(obj.get())->rev();
    else if (obj->type() == earl::value::Type::Tuple)
        return dynamic_cast<earl::value::Tuple *>(obj.get())->rev();
    else
        return dynamic_cast<earl::value::Str *>(obj.get())->rev();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_append(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(values, "append", expr);
    if (obj->type() == earl::value::Type::List)
        dynamic_cast<earl::value::List *>(obj.get())->append(values);
    else
        dynamic_cast<earl::value::Str *>(obj.get())->append(values, expr);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_pop(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                 std::shared_ptr<Ctx> &ctx,
                                 Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(values, 1, "pop", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(values[0], earl::value::Type::Int, 1, "pop", expr);
    if (obj->type() == earl::value::Type::List)
        dynamic_cast<earl::value::List *>(obj.get())->pop(values[0].get());
    else
        dynamic_cast<earl::value::Str *>(obj.get())->pop(values[0].get(), expr);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_contains(std::shared_ptr<earl::value::Obj> obj,
                                      std::vector<std::shared_ptr<earl::value::Obj>> &value,
                                      std::shared_ptr<Ctx> &ctx,
                                      Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(value, 1, "contains", expr);

    if (obj->type() == earl::value::Type::List)
        return dynamic_cast<earl::value::List *>(obj.get())->contains(value[0].get());
    else if (obj->type() == earl::value::Type::Str) {
        if (value[0]->type() != earl::value::Type::Char) {
            Err::err_wexpr(expr);
            const std::string msg = "cannot use member intrinsic `contains` on type str with a argument other than of type char";
            throw InterpreterException(msg);
        }
        auto ch = std::dynamic_pointer_cast<earl::value::Char>(value[0]);
        return dynamic_cast<earl::value::Str *>(obj.get())->contains(ch.get());
    }
    else if (obj->type() == earl::value::Type::Tuple)
        return dynamic_cast<earl::value::Tuple *>(obj.get())->contains(value[0].get());
    else {
        Err::err_wexpr(expr);
        const std::string msg = "cannot call intrinsic method `contains` on non list-adjacent type";
        throw InterpreterException(msg);
    }

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_map(std::shared_ptr<earl::value::Obj> obj,
                              std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                              std::shared_ptr<Ctx> &ctx,
                              Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(closure, 1, "map", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(closure[0], earl::value::Type::Closure, 1, "map", expr);
    auto cl = std::dynamic_pointer_cast<earl::value::Closure>(closure[0]);
    return dynamic_cast<earl::value::List *>(obj.get())->map(cl.get(), ctx);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_fold(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(values, 2, "foldr", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(values[0], earl::value::Type::Closure, 1, "foldr", expr);
    auto cl = std::dynamic_pointer_cast<earl::value::Closure>(values[0]);
    return dynamic_cast<earl::value::List *>(obj.get())->fold(cl.get(), values[1], ctx);
}


