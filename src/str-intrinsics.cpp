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

earl::value::Obj *Intrinsics::intrinsic_member_split(earl::value::Obj *obj, std::vector<earl::value::Obj *> &delim, Ctx &ctx) {
    (void)ctx;

    if (delim.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`split` member intrinsic expects 1 arguments but %zu were supplied",
                  delim.size());
    }

    if (obj->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`split` member intrinsic is only defined for `str` types");
    }

    earl::value::Str *str = dynamic_cast<earl::value::Str *>(obj);
    return str->split(delim[0]);
}

earl::value::Obj *Intrinsics::intrinsic_member_substr(earl::value::Obj *obj, std::vector<earl::value::Obj *> &idxs, Ctx &ctx) {
    (void)ctx;

    earl::value::Obj *tmp = obj;

    if (idxs.size() != 2) {
        ERR_WARGS(Err::Type::Fatal, "`substr` member intrinsic expects 2 arguments but %zu were supplied",
                  idxs.size());
    }

    if (obj->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`substr` member intrinsic is only defined for `str` types");
    }

    if (idxs[0]->type() != earl::value::Type::Int) {
        ERR(Err::Type::Fatal, "argument 1 in `substr` expects an `int` value");
    }

    if (idxs[1]->type() != earl::value::Type::Int) {
        ERR(Err::Type::Fatal, "argument 2 in `substr` expects an `int` value");
    }

    auto *idx1 = dynamic_cast<earl::value::Int *>(idxs[0]);
    auto *idx2 = dynamic_cast<earl::value::Int *>(idxs[1]);
    return dynamic_cast<earl::value::Str *>(tmp)->substr(idx1, idx2);
}

earl::value::Obj *Intrinsics::intrinsic_member_remove_lines(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    UNIMPLEMENTED("Intrinsics::intrinsic_member_remove_lines");

    (void)ctx;
    assert(obj->type() == earl::value::Type::Str);
    assert(unused.size() == 0);
    auto *str = dynamic_cast<earl::value::Str *>(obj);
    return str->remove_lines();
}
