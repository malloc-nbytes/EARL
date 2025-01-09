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
Intrinsics::intrinsic_bool_member_functions = {
    {"ifelse", &Intrinsics::intrinsic_member_ifelse},
    {"toggle", &Intrinsics::intrinsic_member_toggle},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_ifelse(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 2, "ifelse", expr);
    return dynamic_cast<earl::value::Bool *>(obj.get())->value() ? params[0] : params[1];
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_toggle(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "toggle", expr);
    dynamic_cast<earl::value::Bool *>(obj.get())->toggle();
    return std::make_shared<earl::value::Void>();
}
