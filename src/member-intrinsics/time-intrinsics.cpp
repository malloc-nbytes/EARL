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
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_time_member_functions = {
    {"readable", &Intrinsics::intrinsic_member_readable},
    {"years", &Intrinsics::intrinsic_member_years},
    {"months", &Intrinsics::intrinsic_member_months},
    {"days", &Intrinsics::intrinsic_member_days},
    {"hours", &Intrinsics::intrinsic_member_hours},
    {"minutes", &Intrinsics::intrinsic_member_minutes},
    {"seconds", &Intrinsics::intrinsic_member_seconds},
    {"raw", &Intrinsics::intrinsic_member_raw},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_raw(std::shared_ptr<earl::value::Obj> obj,
                                      std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                      std::shared_ptr<Ctx> &ctx,
                                      Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "raw", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->raw();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_readable(std::shared_ptr<earl::value::Obj> obj,
                                      std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                      std::shared_ptr<Ctx> &ctx,
                                      Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "pretty", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->readable();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_years(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "years", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->years();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_months(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "months", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->months();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_days(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx,
                                  Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "days", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->days();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_hours(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "hours", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->hours();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_minutes(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "minutes", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->minutes();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_seconds(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "seconds", expr);
    return dynamic_cast<earl::value::Time *>(obj.get())->seconds();
}

