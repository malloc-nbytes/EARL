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

earl::value::Obj *Intrinsics::intrinsic_member_read(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;
    (void)obj;
    (void)unused;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_read");
}

earl::value::Obj *Intrinsics::intrinsic_member_write(earl::value::Obj *obj, std::vector<earl::value::Obj *> &param, Ctx &ctx) {
    (void)ctx;

    if (param.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`write` member intrinsic expects 1 argument but %zu were supplied",
                  param.size());
    }

    if (obj->type() != earl::value::Type::File) {
        ERR(Err::Type::Fatal, "`write` member intrinsic is only defined for `file` types");
    }

    auto *f = dynamic_cast<earl::value::File *>(obj);
    f->write(param[0]);

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_writelines(earl::value::Obj *obj, std::vector<earl::value::Obj *> &param, Ctx &ctx) {
    (void)ctx;

    if (param.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`writelines` member intrinsic expects 1 argument but %zu were supplied",
                  param.size());
    }

    if (param[0]->type() != earl::value::Type::List) {
        ERR(Err::Type::Fatal, "argument 1 in `substr` expects an `int` value");
    }

    if (obj->type() != earl::value::Type::File) {
        ERR(Err::Type::Fatal, "`writelines` member intrinsic is only defined for `file` types");
    }

    auto *f = dynamic_cast<earl::value::File *>(obj);
    auto *lst = dynamic_cast<earl::value::List *>(param[0]);
    f->writelines(lst);

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_dump(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR(Err::Type::Fatal, "`dump` member intrinsic expects 0 arguments");
    }

    if (obj->type() != earl::value::Type::File) {
        ERR(Err::Type::Fatal, "`dump` member intrinsic is only defined for `file` types");
    }

    auto *f = dynamic_cast<earl::value::File *>(obj);
    f->dump();

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_open(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    (void)expr;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic_open");
}

earl::value::Obj *Intrinsics::intrinsic_member_close(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`close` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::File) {
        ERR(Err::Type::Fatal, "`close` member intrinsic is only defined for `file` types");
    }

    auto *f = dynamic_cast<earl::value::File *>(obj);
    f->close();
    return new earl::value::Void();
}
