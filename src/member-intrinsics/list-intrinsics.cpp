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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

#include "intrinsics.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"
#include "utils.hpp"
#include "common.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_list_member_functions = {
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"back", &Intrinsics::intrinsic_member_back},
    {"filter", &Intrinsics::intrinsic_member_filter},
    {"foreach", &Intrinsics::intrinsic_member_foreach},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
    {"pop", &Intrinsics::intrinsic_member_pop},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_nth(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &idx,
                                 std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(idx, "nth");
    __INTR_ARG_MUSTBE_TYPE_COMPAT(idx[0], earl::value::Type::Int, 1, "nth");
    if (obj->type() == earl::value::Type::List) {
        earl::value::List *list = dynamic_cast<earl::value::List *>(obj.get());
        return list->nth(idx[0]);
    }
    else if (obj->type() == earl::value::Type::Str) {
        earl::value::Str *str = dynamic_cast<earl::value::Str *>(obj.get());
        return str->nth(idx[0]);
    }
    else
        ERR(Err::Type::Fatal, "`nth` member intrinsic is only defined for `list` and `str` types");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_back(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "back");
    if (obj->type() == earl::value::Type::List)
        return dynamic_cast<earl::value::List *>(obj.get())->back();
    else
        return dynamic_cast<earl::value::Str *>(obj.get())->back();
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_filter(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                                    std::shared_ptr<Ctx> &ctx) {
    __INTR_ARGS_MUSTBE_SIZE(closure, 1, "filter");
    __INTR_ARG_MUSTBE_TYPE_COMPAT(closure[0], earl::value::Type::Closure, 1, "filter");
    if (obj->type() == earl::value::Type::List)
        return dynamic_cast<earl::value::List *>(obj.get())->filter(closure.at(0), ctx);
    else
        return dynamic_cast<earl::value::Str *>(obj.get())->filter(closure.at(0), ctx);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_foreach(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                                     std::shared_ptr<Ctx> &ctx) {
    __INTR_ARGS_MUSTBE_SIZE(closure, 1, "foreach");
    __INTR_ARG_MUSTBE_TYPE_COMPAT(closure[0], earl::value::Type::Closure, 1, "foreach");
    if (obj->type() == earl::value::Type::List)
        dynamic_cast<earl::value::List *>(obj.get())->foreach(closure.at(0), ctx);
    else
        dynamic_cast<earl::value::Str *>(obj.get())->foreach(closure.at(0), ctx);
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_rev(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                 std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "rev");
    if (obj->type() == earl::value::Type::List)
        dynamic_cast<earl::value::List *>(obj.get())->rev();
    else
        dynamic_cast<earl::value::Str *>(obj.get())->rev();
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_append(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                    std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(values, "append");
    if (obj->type() == earl::value::Type::List)
        dynamic_cast<earl::value::List *>(obj.get())->append(values);
    else
        dynamic_cast<earl::value::Str *>(obj.get())->append(values);
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_pop(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                 std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(values, 1, "pop");
    __INTR_ARG_MUSTBE_TYPE_COMPAT(values[0], earl::value::Type::Int, 1, "pop");
    if (obj->type() == earl::value::Type::List)
        dynamic_cast<earl::value::List *>(obj.get())->pop(values[0]);
    else
        dynamic_cast<earl::value::Str *>(obj.get())->pop(values[0]);
    return nullptr;
}
