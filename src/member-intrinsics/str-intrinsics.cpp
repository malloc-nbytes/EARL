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

#include "intrinsics.hpp"
#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_str_member_functions = {
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"back", &Intrinsics::intrinsic_member_back},
    {"filter", &Intrinsics::intrinsic_member_filter},
    {"foreach", &Intrinsics::intrinsic_member_foreach},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
    {"pop", &Intrinsics::intrinsic_member_pop},
    {"split", &Intrinsics::intrinsic_member_split},
    {"substr", &Intrinsics::intrinsic_member_substr},
    {"trim", &Intrinsics::intrinsic_member_trim},
    {"contains", &Intrinsics::intrinsic_member_contains},
    {"startswith", &Intrinsics::intrinsic_member_startswith},
    {"endswith", &Intrinsics::intrinsic_member_endswith},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_trim(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx,
                                  Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "trim", expr);
    return dynamic_cast<earl::value::Str *>(obj.get())->trim(expr);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_split(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &delim,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(delim, 1, "split", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(delim[0], earl::value::Type::Str, 1, "split", expr);
    auto str = dynamic_cast<earl::value::Str *>(obj.get());
    return str->split(delim[0].get(), expr);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_substr(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &idxs,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(idxs, 2, "substr", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(idxs[0], earl::value::Type::Int, 1, "substr", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(idxs[1], earl::value::Type::Int, 2, "substr", expr);
    return dynamic_cast<earl::value::Str *>(obj.get())->substr(idxs[0].get(), idxs[1].get(), expr);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_remove_lines(std::shared_ptr<earl::value::Obj> obj,
                                          std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                          std::shared_ptr<Ctx> &ctx,
                                          Expr *expr) {
    (void)obj;
    (void)unused;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_remove_lines");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_startswith(std::shared_ptr<earl::value::Obj> obj,
                                        std::vector<std::shared_ptr<earl::value::Obj>> &str,
                                        std::shared_ptr<Ctx> &ctx,
                                        Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(str, 1, "startswith", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(str[0], earl::value::Type::Str, 1, "startswith", expr);
    return dynamic_cast<earl::value::Str *>(obj.get())->startswith(dynamic_cast<earl::value::Str *>(str[0].get()));
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_endswith(std::shared_ptr<earl::value::Obj> obj,
                                      std::vector<std::shared_ptr<earl::value::Obj>> &str,
                                      std::shared_ptr<Ctx> &ctx,
                                      Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(str, 1, "startswith", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(str[0], earl::value::Type::Str, 1, "startswith", expr);
    return dynamic_cast<earl::value::Str *>(obj.get())->endswith(dynamic_cast<earl::value::Str *>(str[0].get()));
}
