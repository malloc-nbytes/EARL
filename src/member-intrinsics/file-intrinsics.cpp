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
#include "utils.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_file_member_functions = {
    {"dump", &Intrinsics::intrinsic_member_dump},
    {"close", &Intrinsics::intrinsic_member_close},
    {"read", &Intrinsics::intrinsic_member_read},
    {"write", &Intrinsics::intrinsic_member_write},
    {"writelines", &Intrinsics::intrinsic_member_writelines},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_read(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx,
                                  Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "read", expr);
    auto f = dynamic_cast<earl::value::File *>(obj.get());
    return f->read();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_write(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &param,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(param, 1, "write", expr);
    auto f = dynamic_cast<earl::value::File *>(obj.get());
    f->write(param[0]);
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_writelines(std::shared_ptr<earl::value::Obj> obj,
                                        std::vector<std::shared_ptr<earl::value::Obj>> &param,
                                        std::shared_ptr<Ctx> &ctx,
                                        Expr *expr) {
    (void)ctx;
    (void)obj;
    (void)param;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_writelines");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_dump(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx,
                                  Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "dump", expr);
    auto *f = dynamic_cast<earl::value::File *>(obj.get());
    f->dump();
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_close(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "close", expr);
    auto *f = dynamic_cast<earl::value::File *>(obj.get());
    f->close();
    return nullptr;
}
