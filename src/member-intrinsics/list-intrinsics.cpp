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

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_member_nth(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &idx, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)idx;
    (void)obj;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_nth");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_member_back(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)unused;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_back");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_member_filter(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &closure, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)closure;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_filter");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_member_foreach(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &closure, std::shared_ptr<Ctx> &ctx) {
    (void)obj;
    (void)closure;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_foreach");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_member_rev(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)unused;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_rev");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_member_append(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &values, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)values;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_append");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_member_pop(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &values, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)obj;
    (void)values;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_pop");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_len(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic_len");
}
