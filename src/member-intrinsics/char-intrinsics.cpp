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
Intrinsics::intrinsic_char_member_functions = {
    {"ascii", &Intrinsics::intrinsic_member_ascii},
    {"islower", &Intrinsics::intrinsic_member_islower},
    {"isupper", &Intrinsics::intrinsic_member_isupper},
    {"toupper", &Intrinsics::intrinsic_member_toupper},
    {"tolower", &Intrinsics::intrinsic_member_tolower},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_ascii(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    (void)unused;
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "ascii", expr);
    auto char_ = dynamic_cast<earl::value::Char *>(obj.get());
    int value = static_cast<int>(char_->value());
    return std::make_shared<earl::value::Int>(value);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_isupper(std::shared_ptr<earl::value::Obj> obj,
                         std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                         std::shared_ptr<Ctx> &ctx,
                         Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "isupper", expr);
    return dynamic_cast<earl::value::Char *>(obj.get())->isupper();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_islower(std::shared_ptr<earl::value::Obj> obj,
                         std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                         std::shared_ptr<Ctx> &ctx,
                         Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "islower", expr);
    return dynamic_cast<earl::value::Char *>(obj.get())->islower();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_tolower(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "tolower", expr);
    return dynamic_cast<earl::value::Char *>(obj.get())->tolower();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_toupper(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "toupper", expr);
    return dynamic_cast<earl::value::Char *>(obj.get())->toupper();
}
