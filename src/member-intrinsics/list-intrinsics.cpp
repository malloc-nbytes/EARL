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
    (void)idx;
    (void)obj;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_nth");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_back(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)unused;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_back");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_filter(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                                    std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)closure;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_filter");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_foreach(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                                     std::shared_ptr<Ctx> &ctx) {
    (void)obj;
    (void)closure;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_foreach");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_rev(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                 std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)unused;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_rev");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_append(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                    std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)values;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_append");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_pop(std::shared_ptr<earl::value::Obj> obj,
                                 std::vector<std::shared_ptr<earl::value::Obj>> &values,
                                 std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)values;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_pop");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_len(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx) {
    __MEMBER_INTR_ARGS_MUSTBE_SIZE(params, 1, "len");
    __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR(params[0], earl::value::Type::List, earl::value::Type::Str, 1, "len");
    auto &item = params[0];
    if (item->type() == earl::value::Type::List) {
        size_t sz = dynamic_cast<earl::value::List *>(item.get())->value().size();
        return std::make_shared<earl::value::Int>(static_cast<int>(sz));
    }
    else if (item->type() == earl::value::Type::Str) {
        size_t sz = dynamic_cast<earl::value::Str *>(item.get())->value().size();
        return std::make_shared<earl::value::Int>(static_cast<int>(sz));
    }
    assert(false && "unreachable");
    return nullptr;
}
