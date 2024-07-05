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

earl::value::Obj *Intrinsics::call(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    const std::string &id = expr->m_id->lexeme();
    return Intrinsics::intrinsic_functions.at(id)(expr, params, ctx);
}

bool Intrinsics::is_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_functions.find(id) != Intrinsics::intrinsic_functions.end();
}

bool Intrinsics::is_member_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_member_functions.find(id) != Intrinsics::intrinsic_member_functions.end();
}

earl::value::Obj *Intrinsics::call_member(const std::string &id, earl::value::Obj *accessor, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    return Intrinsics::intrinsic_member_functions.at(id)(accessor, params, ctx);
}

earl::value::Obj *Intrinsics::intrinsic_argv(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    (void)expr;

    if (params.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`argv` intrinsic expects 0 arguments but %zu were supplied",
                  params.size());
    }

    std::vector<earl::value::Obj *> args;
    for (size_t i = 0; i < earl_argv.size(); ++i) {
        args.push_back(new earl::value::Str(earl_argv.at(i)));
    }
    return new earl::value::List(std::move(args));
}

earl::value::Obj *Intrinsics::intrinsic___internal_move__(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    (void)expr;
    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`__internal_move__` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    auto *ret = params[0]->copy();
    delete params[0];
    return ret;
}

earl::value::Obj *Intrinsics::intrinsic___internal_mkdir__(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`__internal_mkdir__` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    auto *obj = params[0];
    std::string path = obj->to_cxxstring();

    if (!std::filesystem::exists(path)) {
        if (!std::filesystem::create_directory(path)) {
            ERR_WARGS(Err::Type::Fatal, "could not create directory `%s`", path.c_str());
        }
    }

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_type(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)expr;
    (void)ctx;

    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`type` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    return new earl::value::Str(earl::value::type_to_str(params[0]));
}

earl::value::Obj *Intrinsics::intrinsic_unimplemented(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    std::cout << "[EARL] UNIMPLEMENTED";

    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(expr, params, ctx);
    }

    exit(1);

    return nullptr; // unreachable
}

earl::value::Obj *Intrinsics::intrinsic_exit(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    (void)expr;
    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`exit` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    if (params[0]->type() != earl::value::Type::Int) {
        ERR(Err::Type::Fatal, "argument 1 in `exit` expects an `int` value");
    }

    exit(dynamic_cast<earl::value::Int *>(params[0])->value());
}

earl::value::Obj *Intrinsics::intrinsic_panic(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    std::cout << "[EARL] PANIC";

    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(expr, params, ctx);
    }

    exit(1);

    return nullptr; // unreachable
}

earl::value::Obj *Intrinsics::intrinsic_assert(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;

    for (size_t i = 0; i < params.size(); ++i) {
        earl::value::Obj *param = params.at(i);
        if (!param->boolean()) {
            Err::err_wtok(expr->m_id.get());
            token_dump_until_eol(expr->m_id.get());
            ERR_WARGS(Err::Type::Assertion,
                      "assertion failure (expression=%zu) (earl::value::Type=%d)",
                      i+1, (int)param->type());
        }
    }

    return new earl::value::Void();
}

static void __intrinsic_print(ExprFuncCall *expr, earl::value::Obj *param) {
    switch (param->type()) {
    case earl::value::Type::Int: {
        auto *intparam = dynamic_cast<earl::value::Int *>(param);
        std::cout << intparam->value();
    } break;
    case earl::value::Type::Bool: {
        auto *bool_param = dynamic_cast<earl::value::Bool *>(param);
        std::cout << (bool_param->value() ? "true" : "false");
    } break;
    case earl::value::Type::Option: {
        auto *option_param = dynamic_cast<earl::value::Option *>(param);

        if (option_param->is_none())
            std::cout << "<none>";
        else {
            std::cout << "some(";
            __intrinsic_print(expr, option_param->value());
            std::cout << ")";
        }

    } break;
    case earl::value::Type::Str: {
        auto *strparam = dynamic_cast<earl::value::Str *>(param);
        std::cout << strparam->value();
    } break;
    case earl::value::Type::Char: {
        auto *strparam = dynamic_cast<earl::value::Char *>(param);
        std::cout << strparam->value();
    } break;
    case earl::value::Type::Class: {
        auto *classparam = dynamic_cast<earl::value::Class *>(param);
        std::cout << "<Class " << classparam->id() << " " << &classparam << '>';
    } break;
    case earl::value::Type::List: {
        earl::value::List *listparam = dynamic_cast<earl::value::List *>(param);
        std::cout << '[';
        std::vector<earl::value::Obj *> &list = listparam->value();

        for (size_t i = 0; i < list.size(); ++i) {
            __intrinsic_print(expr, list[i]);

            if (i != list.size()-1) {
                std::cout << ", ";
            }
        }

        std::cout << ']';
    } break;
    default: {
        Err::err_wtok(expr->m_id.get());
        ERR_WARGS(Err::Type::Fatal, "intrinsic_print: unknown parameter type %d", static_cast<int>(param->type()));
    } break;
    }
}

earl::value::Obj *Intrinsics::intrinsic_print(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i) {
        __intrinsic_print(expr, params[i]);
    }
    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_println(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i) {
        __intrinsic_print(expr, params[i]);
    }
    std::cout << '\n';
    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_input(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    intrinsic_print(expr, params, ctx);
    std::string in = "";
    std::getline(std::cin, in);
    return new earl::value::Str(in);
}
