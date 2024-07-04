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

earl::value::Obj *Intrinsics::intrinsic_some(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)expr;
    (void)ctx;

    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`some` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    return new earl::value::Option(params[0]);
}

earl::value::Obj *Intrinsics::intrinsic_member_unwrap(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`unwrap` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::Option) {
        ERR(Err::Type::Fatal, "`unwrap` member intrinsic is only defined for `option` types");
    }

    auto *none = dynamic_cast<earl::value::Option *>(obj);

    if (!none->value()) {
        ERR(Err::Type::Fatal, "tried to unwrap none value");
    }

    return none->value();
}

earl::value::Obj *Intrinsics::intrinsic_member_is_none(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`is_none` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::Option) {
        ERR(Err::Type::Fatal, "`is_none` member intrinsic is only defined for `option` types");
    }

    return new earl::value::Bool(dynamic_cast<earl::value::Option *>(obj)->is_none());
}

earl::value::Obj *Intrinsics::intrinsic_member_is_some(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`is_some` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::Option) {
        ERR(Err::Type::Fatal, "`is_some` member intrinsic is only defined for `option` types");
    }

    return new earl::value::Bool(dynamic_cast<earl::value::Option *>(obj)->is_some());
}
