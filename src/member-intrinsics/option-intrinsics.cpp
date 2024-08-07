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
#include <fstream>

#include "intrinsics.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"
#include "utils.hpp"
#include "common.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_option_member_functions = {
    {"unwrap", &Intrinsics::intrinsic_member_unwrap},
    {"is_none", &Intrinsics::intrinsic_member_is_none},
    {"is_some", &Intrinsics::intrinsic_member_is_some},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_unwrap(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                    std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "unwrap");
    auto none = dynamic_cast<earl::value::Option *>(obj.get());

    if (!none->value()) {
        ERR(Err::Type::Fatal, "tried to unwrap none value");
    }

    return none->value();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_is_none(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "is_none");
    return std::make_shared<earl::value::Bool>(dynamic_cast<earl::value::Option *>(obj.get())->is_none());
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_is_some(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                     std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "is_some");
    return std::make_shared<earl::value::Bool>(dynamic_cast<earl::value::Option *>(obj.get())->is_some());
}
