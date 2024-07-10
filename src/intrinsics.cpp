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
#include <filesystem>

#include "intrinsics.hpp"
#include "interpreter.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"
#include "utils.hpp"
#include "common.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicFunction> Intrinsics::intrinsic_functions = {
    {"print", &Intrinsics::intrinsic_print},
    {"println", &Intrinsics::intrinsic_println},
    {"assert", &Intrinsics::intrinsic_assert},
    {"len", &Intrinsics::intrinsic_len},
    {"open", &Intrinsics::intrinsic_open},
    {"type", &Intrinsics::intrinsic_type},
    {"unimplemented", &Intrinsics::intrinsic_unimplemented},
    {"exit", &Intrinsics::intrinsic_exit},
    {"panic", &Intrinsics::intrinsic_panic},
    {"some", &Intrinsics::intrinsic_some},
    {"argv", &Intrinsics::intrinsic_argv},
    {"input", &Intrinsics::intrinsic_input},
    {"__internal_move__", &Intrinsics::intrinsic___internal_move__},
    {"__internal_mkdir__", &Intrinsics::intrinsic___internal_mkdir__},
    {"__internal_ls__", &Intrinsics::intrinsic___internal_ls__},
};

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> Intrinsics::intrinsic_member_functions = {
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"back", &Intrinsics::intrinsic_member_back},
    {"filter", &Intrinsics::intrinsic_member_filter},
    {"foreach", &Intrinsics::intrinsic_member_foreach},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
    {"pop", &Intrinsics::intrinsic_member_pop},
    {"split", &Intrinsics::intrinsic_member_split},
    {"substr", &Intrinsics::intrinsic_member_substr},
    {"remove_lines", &Intrinsics::intrinsic_member_remove_lines},
    {"dump", &Intrinsics::intrinsic_member_dump},
    {"close", &Intrinsics::intrinsic_member_close},
    {"read", &Intrinsics::intrinsic_member_read},
    {"write", &Intrinsics::intrinsic_member_write},
    {"writelines", &Intrinsics::intrinsic_member_writelines},
    {"ascii", &Intrinsics::intrinsic_member_ascii},
    {"unwrap", &Intrinsics::intrinsic_member_unwrap},
    {"is_none", &Intrinsics::intrinsic_member_is_none},
    {"is_some", &Intrinsics::intrinsic_member_is_some},
};

std::shared_ptr<earl::value::Obj> Intrinsics::call(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::call");
}

bool Intrinsics::is_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_functions.find(id) != Intrinsics::intrinsic_functions.end();
}

bool Intrinsics::is_member_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_member_functions.find(id) != Intrinsics::intrinsic_member_functions.end();
}

std::shared_ptr<earl::value::Obj> Intrinsics::call_member(const std::string &id, std::shared_ptr<earl::value::Obj> accessor, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    return Intrinsics::intrinsic_member_functions.at(id)(accessor, params, ctx);
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_argv(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)expr;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic_argv");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic___internal_move__(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)expr;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic___internal_move__");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic___internal_mkdir__(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic___internal_mkdir__");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic___internal_ls__(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic___internal_ls__");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_type(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)ctx;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic_type");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_unimplemented(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    std::cout << "[EARL] UNIMPLEMENTED";

    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(expr, params, ctx);
    }

    exit(1);

    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_exit(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)expr;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic_exit");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_panic(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    std::cout << "[EARL] PANIC";

    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(expr, params, ctx);
    }

    exit(1);

    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_assert(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)params;
    (void)expr;
    UNIMPLEMENTED("Intrinsics::intrinsic_assert");
}

static void __intrinsic_print(ExprFuncCall *expr, earl::value::Obj *param) {
    (void)expr;
    (void)param;
    UNIMPLEMENTED("__intrinsic_print");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_print(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)expr;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic_print");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_println(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)expr;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic_println");
}

std::shared_ptr<earl::value::Obj> Intrinsics::intrinsic_input(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, std::shared_ptr<Ctx> &ctx) {
    (void)expr;
    (void)params;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic_input");
}
