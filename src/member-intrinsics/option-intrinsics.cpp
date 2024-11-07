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
Intrinsics::intrinsic_option_member_functions = {
    {"unwrap", &Intrinsics::intrinsic_member_unwrap},
    {"is_none", &Intrinsics::intrinsic_member_is_none},
    {"is_some", &Intrinsics::intrinsic_member_is_some},
    {"unwrap_or", &Intrinsics::intrinsic_member_unwrap_or},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_unwrap(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "unwrap", expr);
    auto none = dynamic_cast<earl::value::Option *>(obj.get());

    if (!none->value()) {
        Err::err_wexpr(expr);
        std::string msg = "tried to unwrap none value";
        throw InterpreterException(msg);
    }

    return none->value();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_is_none(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "is_none", expr);
    return std::make_shared<earl::value::Bool>(dynamic_cast<earl::value::Option *>(obj.get())->is_none());
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_is_some(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "is_some", expr);
    return std::make_shared<earl::value::Bool>(dynamic_cast<earl::value::Option *>(obj.get())->is_some());
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_unwrap_or(std::shared_ptr<earl::value::Obj> obj,
                                       std::vector<std::shared_ptr<earl::value::Obj>> &or_value,
                                       std::shared_ptr<Ctx> &ctx,
                                       Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(or_value, 1, "unwrap_or", expr);
    auto o = dynamic_cast<earl::value::Option *>(obj.get());
    if (o->is_none())
        return or_value[0];
    else
        return o->value();
}
