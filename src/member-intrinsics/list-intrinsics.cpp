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

earl::value::Obj *Intrinsics::intrinsic_member_nth(earl::value::Obj *obj, std::vector<earl::value::Obj *> &idx, Ctx &ctx) {
    (void)ctx;

    if (idx.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`nth` member intrinsic expects 1 argument but %zu were supplied",
                  idx.size());
    }

    if (obj->type() == earl::value::Type::List) {
        earl::value::List *list = dynamic_cast<earl::value::List *>(obj);
        return list->nth(idx[0]);
    }
    else if (obj->type() == earl::value::Type::Str) {
        earl::value::Str *str = dynamic_cast<earl::value::Str *>(obj);
        return str->nth(idx[0]);
    }
    else {
        ERR(Err::Type::Fatal, "`nth` member intrinsic is only defined for `list` and `str` types");
    }
}

earl::value::Obj *Intrinsics::intrinsic_member_back(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`back` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    return dynamic_cast<earl::value::List *>(obj)->back();
}

earl::value::Obj *Intrinsics::intrinsic_member_filter(earl::value::Obj *obj, std::vector<earl::value::Obj *> &closure, Ctx &ctx) {
    if (closure.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`filter` member intrinsic expects 1 argument but %zu were supplied",
                  closure.size());
    }

    if (closure[0]->type() != earl::value::Type::Closure) {
        ERR(Err::Type::Fatal, "argument 1 in `filter` expects a `closure` value");
    }

    return dynamic_cast<earl::value::List *>(obj)->filter(closure.at(0), ctx);
}

earl::value::Obj *Intrinsics::intrinsic_member_foreach(earl::value::Obj *obj, std::vector<earl::value::Obj *> &closure, Ctx &ctx) {
    if (closure.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`foreach` member intrinsic expects 1 argument but %zu were supplied",
                  closure.size());
    }

    if (closure[0]->type() != earl::value::Type::Closure) {
        ERR(Err::Type::Fatal, "argument 1 in `foreach` expects a `closure` value");
    }

    dynamic_cast<earl::value::List *>(obj)->foreach(closure.at(0), ctx);

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_rev(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`rev` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::List) {
        ERR(Err::Type::Fatal, "`rev` member intrinsic is only defined for `list` types");
    }

    auto *list = dynamic_cast<earl::value::List *>(obj);
    list->rev();

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_append(earl::value::Obj *obj, std::vector<earl::value::Obj *> &values, Ctx &ctx) {
    (void)ctx;

    if (values.size() == 0) {
        ERR_WARGS(Err::Type::Fatal, "`append` member intrinsic expects variadic arguments but %zu were supplied",
                  values.size());
    }

    if (obj->type() != earl::value::Type::List) {
        ERR(Err::Type::Fatal, "`append` member intrinsic is only defined for `list` types");
    }

    auto *lst = dynamic_cast<earl::value::List *>(obj);
    return lst->append(values);
}

earl::value::Obj *Intrinsics::intrinsic_member_pop(earl::value::Obj *obj, std::vector<earl::value::Obj *> &values, Ctx &ctx) {
    (void)ctx;

    if (values.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`pop` member intrinsic expects 1 argument but %zu were supplied",
                  values.size());
    }

    if (obj->type() != earl::value::Type::List && obj->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`pop` member intrinsic is only defined for `list` and `str` types");
    }

    if (obj->type() == earl::value::Type::List) {
        auto *lst = dynamic_cast<earl::value::List *>(obj);
        return lst->pop(values[0]);
    }
    else if (obj->type() == earl::value::Type::Str) {
        auto *str = dynamic_cast<earl::value::Str *>(obj);
        return str->pop(values[0]);
    }

    assert(false && "unreachable");
}

earl::value::Obj *Intrinsics::intrinsic_len(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)expr;
    (void)ctx;

    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`len` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    if (params[0]->type() != earl::value::Type::List && params[0]->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`len` intrinsic is only defined for `list` and `str` types");
    }

    if (params[0]->type() == earl::value::Type::List) {
        return new earl::value::Int(dynamic_cast<earl::value::List *>(params[0])->value().size());
    }
    else if (params[0]->type() == earl::value::Type::Str) {
        return new earl::value::Int(dynamic_cast<earl::value::Str *>(params[0])->value().size());
    }

    ERR_WARGS(Err::Type::Fatal, "canot call `len` on unsupported type (%d)", (int)params[0]->type());
    return nullptr;
}
