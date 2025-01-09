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

#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <unistd.h>
#include <random>
#include <variant>

#include "intrinsics.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "earl.hpp"
#include "common.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicFunction>
Intrinsics::intrinsic_functions = {
    {"flush", &Intrinsics::intrinsic_flush},
    {"unset_flag", &Intrinsics::intrinsic_unset_flag},
    {"set_flag", &Intrinsics::intrinsic_set_flag},
    {"sin", &Intrinsics::intrinsic_sin},
    {"cos", &Intrinsics::intrinsic_cos},
    {"help", &Intrinsics::intrinsic_help},
    {"print", &Intrinsics::intrinsic_print},
    {"println", &Intrinsics::intrinsic_println},
    {"assert", &Intrinsics::intrinsic_assert},
    {"len", &Intrinsics::intrinsic_len},
    {"copy", &Intrinsics::intrinsic_copy},
    {"open", &Intrinsics::intrinsic_open},
    {"type", &Intrinsics::intrinsic_type},
    {"typeof", &Intrinsics::intrinsic_typeof},
    {"unimplemented", &Intrinsics::intrinsic_unimplemented},
    {"exit", &Intrinsics::intrinsic_exit},
    {"warn", &Intrinsics::intrinsic_warn},
    {"panic", &Intrinsics::intrinsic_panic},
    {"some", &Intrinsics::intrinsic_some},
    {"argv", &Intrinsics::intrinsic_argv},
    {"input", &Intrinsics::intrinsic_input},
    {"observe", &Intrinsics::intrinsic_observe},
    {"init_seed", &Intrinsics::intrinsic_init_seed},
    {"random", &Intrinsics::intrinsic_random},
    {"__internal_isdir__", &Intrinsics::intrinsic___internal_isdir__},
    {"__internal_move__", &Intrinsics::intrinsic___internal_move__},
    {"__internal_mkdir__", &Intrinsics::intrinsic___internal_mkdir__},
    {"__internal_ls__", &Intrinsics::intrinsic___internal_ls__},
    {"cd", &Intrinsics::intrinsic_cd},
    {"__internal_unix_system__", &Intrinsics::intrinsic___internal_unix_system__},
    {"__internal_unix_system_woutput__", &Intrinsics::intrinsic___internal_unix_system_woutput__},
    {"fprintln", &Intrinsics::intrinsic_fprintln},
    {"fprint", &Intrinsics::intrinsic_fprint},
    // Casting Functions
    {"str", &Intrinsics::intrinsic_str},
    {"int", &Intrinsics::intrinsic_int},
    {"float", &Intrinsics::intrinsic_float},
    {"bool", &Intrinsics::intrinsic_bool},
    {"tuple", &Intrinsics::intrinsic_tuple},
    {"list", &Intrinsics::intrinsic_list},
    {"unit", &Intrinsics::intrinsic_unit},
    {"Dict", &Intrinsics::intrinsic_Dict},
    {"datetime", &Intrinsics::intrinsic_datetime},
    {"sleep", &Intrinsics::intrinsic_sleep},
    {"env", &Intrinsics::intrinsic_env},
};

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_member_functions = {
    // List/str
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"back", &Intrinsics::intrinsic_member_back},
    {"filter", &Intrinsics::intrinsic_member_filter},
    {"foreach", &Intrinsics::intrinsic_member_foreach},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
    {"pop", &Intrinsics::intrinsic_member_pop},
    {"contains", &Intrinsics::intrinsic_member_contains},
    {"map", &Intrinsics::intrinsic_member_map},
    {"fold", &Intrinsics::intrinsic_member_fold},
    // Str
    {"split", &Intrinsics::intrinsic_member_split},
    {"substr", &Intrinsics::intrinsic_member_substr},
    {"trim", &Intrinsics::intrinsic_member_trim},// UNIMPLEMENTED
    {"remove_lines", &Intrinsics::intrinsic_member_remove_lines},// UNIMPLEMENTED
    // File
    {"dump", &Intrinsics::intrinsic_member_dump},
    {"close", &Intrinsics::intrinsic_member_close},
    {"read", &Intrinsics::intrinsic_member_read},
    {"write", &Intrinsics::intrinsic_member_write},
    {"writelines", &Intrinsics::intrinsic_member_writelines},
    // Char
    {"ascii", &Intrinsics::intrinsic_member_ascii},
    // Int
    {"utf8", &Intrinsics::intrinsic_member_utf8},
    // Option
    {"unwrap", &Intrinsics::intrinsic_member_unwrap},
    {"is_none", &Intrinsics::intrinsic_member_is_none},
    {"is_some", &Intrinsics::intrinsic_member_is_some},
    {"unwrap_or", &Intrinsics::intrinsic_member_unwrap_or},
    // Dict
    {"insert", &Intrinsics::intrinsic_member_insert},
    {"has_key", &Intrinsics::intrinsic_member_has_key},
    {"has_value", &Intrinsics::intrinsic_member_has_value},
    // Bool
    {"ifelse", &Intrinsics::intrinsic_member_ifelse},
    {"toggle", &Intrinsics::intrinsic_member_toggle},
    // Time
    {"readable", &Intrinsics::intrinsic_member_readable},
    {"years", &Intrinsics::intrinsic_member_years},
    {"months", &Intrinsics::intrinsic_member_months},
    {"days", &Intrinsics::intrinsic_member_days},
    {"hours", &Intrinsics::intrinsic_member_hours},
    {"minutes", &Intrinsics::intrinsic_member_minutes},
    {"seconds", &Intrinsics::intrinsic_member_seconds},
    {"raw", &Intrinsics::intrinsic_member_raw},
};


std::shared_ptr<earl::value::Obj>
Intrinsics::call(const std::string &id,
                 std::vector<std::shared_ptr<earl::value::Obj>> &params,
                 std::shared_ptr<Ctx> &ctx,
                 Expr *expr) {
    return intrinsic_functions.at(id)(params, ctx, expr);
}

bool
Intrinsics::is_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_functions.find(id) != Intrinsics::intrinsic_functions.end();
}

bool
Intrinsics::is_member_intrinsic(const std::string &id, int ty) {
    if (ty == -1)
        return Intrinsics::intrinsic_member_functions.find(id) != Intrinsics::intrinsic_member_functions.end();

    switch (static_cast<earl::value::Type>(ty)) {
    case earl::value::Type::Int:       return Intrinsics::intrinsic_int_member_functions.find(id)    != Intrinsics::intrinsic_int_member_functions.end();
    case earl::value::Type::Char:      return Intrinsics::intrinsic_char_member_functions.find(id)   != Intrinsics::intrinsic_char_member_functions.end();
    case earl::value::Type::Str:       return Intrinsics::intrinsic_str_member_functions.find(id)    != Intrinsics::intrinsic_str_member_functions.end();
    case earl::value::Type::Bool:      return Intrinsics::intrinsic_bool_member_functions.find(id)   != Intrinsics::intrinsic_bool_member_functions.end();
    case earl::value::Type::List:      return Intrinsics::intrinsic_list_member_functions.find(id)   != Intrinsics::intrinsic_list_member_functions.end();
    case earl::value::Type::Option:    return Intrinsics::intrinsic_option_member_functions.find(id) != Intrinsics::intrinsic_option_member_functions.end();
    case earl::value::Type::File:      return Intrinsics::intrinsic_file_member_functions.find(id)   != Intrinsics::intrinsic_file_member_functions.end();
    case earl::value::Type::Tuple:     return Intrinsics::intrinsic_tuple_member_functions.find(id)  != Intrinsics::intrinsic_tuple_member_functions.end();
    case earl::value::Type::DictInt:
    case earl::value::Type::DictStr:
    case earl::value::Type::DictChar:
    case earl::value::Type::DictFloat: return Intrinsics::intrinsic_dict_member_functions.find(id)   != Intrinsics::intrinsic_dict_member_functions.end();
    case earl::value::Type::Time:      return Intrinsics::intrinsic_time_member_functions.find(id)   != Intrinsics::intrinsic_time_member_functions.end();
    default: return false;
    }
    return Intrinsics::intrinsic_member_functions.find(id) != Intrinsics::intrinsic_member_functions.end();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::call_member(const std::string &id,
                        earl::value::Type type,
                        std::shared_ptr<earl::value::Obj> accessor,
                        std::vector<std::shared_ptr<earl::value::Obj>> &params,
                        std::shared_ptr<Ctx> &ctx,
                        Expr *expr) {

    switch (type) {
    case earl::value::Type::Int:       return Intrinsics::intrinsic_int_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::Char:      return Intrinsics::intrinsic_char_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::Str:       return Intrinsics::intrinsic_str_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::Bool:      return Intrinsics::intrinsic_bool_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::List:      return Intrinsics::intrinsic_list_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::Option:    return Intrinsics::intrinsic_option_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::File:      return Intrinsics::intrinsic_file_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::Tuple:     return Intrinsics::intrinsic_tuple_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::DictInt:
    case earl::value::Type::DictStr:
    case earl::value::Type::DictChar:
    case earl::value::Type::DictFloat: return Intrinsics::intrinsic_dict_member_functions.at(id)(accessor, params, ctx, expr);
    case earl::value::Type::Time:      return Intrinsics::intrinsic_time_member_functions.at(id)(accessor, params, ctx, expr);
    default: assert(false);
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_str(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx,
                          Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "str", expr);
    return std::make_shared<earl::value::Str>(params[0]->to_cxxstring());
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_int(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx,
                          Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "int", expr);
    {
        std::vector<earl::value::Type> tys = {
            earl::value::Type::Int,
            earl::value::Type::Float,
            earl::value::Type::Str,
            earl::value::Type::Bool,
        };
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], tys, 1, "int", expr);
    }
    switch (params[0]->type()) {
    case earl::value::Type::Int: {
        int i = dynamic_cast<earl::value::Int *>(params[0].get())->value();
        return std::make_shared<earl::value::Int>(i);
    } break;
    case earl::value::Type::Float: {
        double f = dynamic_cast<earl::value::Float *>(params[0].get())->value();
        return std::make_shared<earl::value::Int>(static_cast<int>(f));
    } break;
    case earl::value::Type::Str: {
        std::string s = dynamic_cast<earl::value::Str *>(params[0].get())->value();
        return std::make_shared<earl::value::Int>(std::stoi(s));
    } break;
    case earl::value::Type::Char: {
        char c = dynamic_cast<earl::value::Char *>(params[0].get())->value();
        return std::make_shared<earl::value::Int>(c-'0');
    } break;
    case earl::value::Type::Bool: {
        bool b = dynamic_cast<earl::value::Bool *>(params[0].get())->value();
        return std::make_shared<earl::value::Int>(static_cast<int>(b));
    } break;
    default: {
        Err::err_wexpr(expr);
        std::string msg = "cannot convert type `"+earl::value::type_to_str(params[0]->type())+"` to type int";
        throw InterpreterException(msg);
    } break;
    }
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_float(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "float", expr);
    {
        std::vector<earl::value::Type> tys = {
            earl::value::Type::Int,
            earl::value::Type::Float,
            earl::value::Type::Str,
        };
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], tys, 1, "float", expr);
    }
    switch (params[0]->type()) {
    case earl::value::Type::Int: {
        int i = dynamic_cast<earl::value::Int *>(params[0].get())->value();
        return std::make_shared<earl::value::Float>(static_cast<double>(i));
    } break;
    case earl::value::Type::Float: {
        double f = dynamic_cast<earl::value::Float *>(params[0].get())->value();
        return std::make_shared<earl::value::Float>(f);
    } break;
    case earl::value::Type::Str: {
        std::string s = dynamic_cast<earl::value::Str *>(params[0].get())->value();
        return std::make_shared<earl::value::Float>(std::stof(s));
    } break;
    default: {
        Err::err_wexpr(expr);
        std::string msg = "cannot convert type `"+earl::value::type_to_str(params[0]->type())+"` to type float";
        throw InterpreterException(msg);
    } break;
    }
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_bool(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "bool", expr);
    {
        std::vector<earl::value::Type> tys = {
            earl::value::Type::Int,
            earl::value::Type::Float,
            earl::value::Type::Str,
        };
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], tys, 1, "bool", expr);
    }
    switch (params[0]->type()) {
    case earl::value::Type::Int: {
        int i = dynamic_cast<earl::value::Int *>(params[0].get())->value();
        return std::make_shared<earl::value::Bool>(static_cast<bool>(i));
    } break;
    case earl::value::Type::Float: {
        double f = dynamic_cast<earl::value::Float *>(params[0].get())->value();
        return std::make_shared<earl::value::Bool>(static_cast<bool>(f));
    } break;
    case earl::value::Type::Str: {
        std::string s = dynamic_cast<earl::value::Str *>(params[0].get())->value();
        if (s == COMMON_EARLKW_TRUE)
            return std::make_shared<earl::value::Bool>(true);
        else if (s == COMMON_EARLKW_FALSE)
            return std::make_shared<earl::value::Bool>(false);
        Err::err_wexpr(expr);
        std::string msg = "cannot convert str `"+s+"` to type bool";
        throw InterpreterException(msg);
    } break;
    default: {
        Err::err_wexpr(expr);
        std::string msg = "cannot convert type `"+earl::value::type_to_str(params[0]->type())+"` to type bool";
        throw InterpreterException(msg);
    } break;
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_tuple(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    (void)ctx;
    return std::make_shared<earl::value::Tuple>(params);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_list(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    return std::make_shared<earl::value::List>(params);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_unit(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    (void)params;
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_Dict(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "Dict", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::TypeKW, 1, "Dict", expr);

    auto value = dynamic_cast<earl::value::TypeKW *>(params[0].get());
    earl::value::Type ty = value->ty();

    switch (ty) {
    case earl::value::Type::Int: return std::make_shared<earl::value::Dict<int>>(ty);
    case earl::value::Type::Str: return std::make_shared<earl::value::Dict<std::string>>(ty);
    case earl::value::Type::Char: return std::make_shared<earl::value::Dict<char>>(ty);
    case earl::value::Type::Float: return std::make_shared<earl::value::Dict<float>>(ty);
    default: {
        Err::err_wexpr(expr);
        const std::string msg = "cannot create an empty dictionary of type `"+earl::value::type_to_str(ty)+"` (unsupported)";
        throw InterpreterException(msg);
    }
    }

    assert(false);
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_flush(std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "flush", expr);
    std::cout << std::flush;
    return std::make_shared<earl::value::Void>();
}


std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_unset_flag(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                 std::shared_ptr<Ctx> &ctx,
                                 Expr *expr) {
    __INTR_ARGS_MUSTNOT_BE_0(params, "unset_flag", expr);
    for (size_t i = 0; i < params.size(); ++i) {
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[i], earl::value::Type::Str, 1, "unset_flag", expr);
        const std::string &flag = dynamic_cast<earl::value::Str *>(params.at(i).get())->value();
        if (flag.size() < 2 || flag.at(0) != '-')
            throw InterpreterException("invalid flag " + flag);
        if (flag.at(1) == '-')
            handle_2flag(flag.substr(2), false);
        else {
            std::string trim = flag.substr(1);
            for (auto c : trim)
                handle_1flag(c, false);
        }
    }
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_set_flag(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                               std::shared_ptr<Ctx> &ctx,
                               Expr *expr) {
    __INTR_ARGS_MUSTNOT_BE_0(params, "set_flag", expr);
    for (size_t i = 0; i < params.size(); ++i) {
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[i], earl::value::Type::Str, 1, "set_flag", expr);
        const std::string &flag = dynamic_cast<earl::value::Str *>(params.at(i).get())->value();
        if (flag.size() < 2 || flag.at(0) != '-')
            throw InterpreterException("invalid flag " + flag);
        if (flag.at(1) == '-')
            handle_2flag(flag.substr(2), true);
        else {
            std::string trim = flag.substr(1);
            for (auto c : trim)
                handle_1flag(c, true);
        }
    }
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_help(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "help", expr);
    auto obj = params[0];

    auto iter_help = [](const std::vector<std::string> &v) {
        std::string total = "";
        if (v.size() == 0)
            total = "<no help information provided>";
        for (size_t i = 0; i < v.size(); ++i) {
            total += v.at(i);
            if (i != v.size()-1)
                total += '\n';
        }
        return std::make_shared<earl::value::Str>(total);
    };

    switch (obj->type()) {
    case earl::value::Type::FunctionRef: {
        const std::vector<std::string> &info = dynamic_cast<earl::value::FunctionRef *>(obj.get())->get_info();
        return iter_help(info);
    } break;
    case earl::value::Type::ClassRef: {
        const std::vector<std::string> &info = dynamic_cast<earl::value::ClassRef *>(obj.get())->get_info();
        return iter_help(info);
    } break;
    case earl::value::Type::Enum: {
        const std::vector<std::string> &info = dynamic_cast<earl::value::Enum *>(obj.get())->get_info();
        return iter_help(info);
    } break;
    default: {
        std::string info = obj->get_info_from_owner();
        if (info == "")
            info = "<no help information provided>";
        return std::make_shared<earl::value::Str>(info);
    }
    }
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_cos(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx,
                          Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "cos", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Int, 1, "cos", expr);
    auto obj = params[0];
    if (obj->type() == earl::value::Type::Int) {
        int value = dynamic_cast<earl::value::Int *>(obj.get())->value();
        return std::make_shared<earl::value::Float>(cos(value));
    }
    double value = dynamic_cast<earl::value::Float *>(obj.get())->value();
    return std::make_shared<earl::value::Float>(cos(value));
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_sin(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx,
                          Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "sin", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Int, 1, "sin", expr);
    auto obj = params[0];
    if (obj->type() == earl::value::Type::Int) {
        int value = dynamic_cast<earl::value::Int *>(obj.get())->value();
        return std::make_shared<earl::value::Float>(sin(value));
    }
    double value = dynamic_cast<earl::value::Float *>(obj.get())->value();
    return std::make_shared<earl::value::Float>(sin(value));
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_len(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx,
                          Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "len", expr);
    {
        std::vector<earl::value::Type> lst = {earl::value::Type::List, earl::value::Type::Str, earl::value::Type::Tuple};
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], lst, 1, "len", expr);
    }
    auto &item = params[0];
    if (item->type() == earl::value::Type::List) {
        size_t sz = dynamic_cast<earl::value::List *>(item.get())->value().size();
        return std::make_shared<earl::value::Int>(static_cast<int>(sz));
    }
    else if (item->type() == earl::value::Type::Str) {
        size_t sz = dynamic_cast<earl::value::Str *>(item.get())->value().size();
        return std::make_shared<earl::value::Int>(static_cast<int>(sz));
    }
    else if (item->type() == earl::value::Type::Tuple) {
        size_t sz = dynamic_cast<earl::value::Tuple *>(item.get())->value().size();
        return std::make_shared<earl::value::Int>(static_cast<int>(sz));
    }
    assert(false && "unreachable");
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_copy(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "copy", expr);
    return params[0]->copy();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_argv(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 0, "argv", expr);
    std::vector<std::shared_ptr<earl::value::Obj>> args = {};
    for (size_t i = 0; i < earl_argv.size(); ++i)
        args.push_back(std::make_shared<earl::value::Str>(earl_argv.at(i)));
    return std::make_shared<earl::value::List>(args);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_isdir__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                         std::shared_ptr<Ctx> &ctx,
                                         Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_isdir__", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT_EXACT(params[0], earl::value::Type::Str, 1, "__internal_isdir__", expr);
    const std::string path = dynamic_cast<earl::value::Str *>(params[0].get())->value();
    return std::make_shared<earl::value::Bool>(std::filesystem::is_directory(path));
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_mkdir__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                         std::shared_ptr<Ctx> &ctx,
                                         Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_mkdir__", expr);
    auto obj = params[0];
    std::string path = obj->to_cxxstring();
    if (!std::filesystem::exists(path))
        if (!std::filesystem::create_directory(path)) {
            Err::err_wexpr(expr);
            std::string msg = "could not create directory `"+path+"`";
            throw InterpreterException(msg);
        }
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_move__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                         std::shared_ptr<Ctx> &ctx,
                                         Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 2, "__internal_move__", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1, "__internal_move__", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[1], earl::value::Type::Str, 2, "__internal_move__", expr);

    auto path_obj = params[0];
    auto to_obj = params[1];
    std::string path_from = path_obj->to_cxxstring();
    std::string path_to = to_obj->to_cxxstring();

    if (std::filesystem::exists(path_from)) {
        std::filesystem::rename(path_from, path_to);
    }
    else {
        Err::err_wexpr(expr);
        std::string msg = "File `"+path_from+"` does not exist";
        throw InterpreterException(msg);
    }

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_ls__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                      std::shared_ptr<Ctx> &ctx,
                                      Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_ls__", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1, "__internal_ls__", expr);

    auto obj = params[0];
    std::string path = obj->to_cxxstring();

    auto lst = std::make_shared<earl::value::List>();
    std::vector<std::shared_ptr<earl::value::Obj>> items={};

    try {
        for (const auto &entry : std::filesystem::directory_iterator(path))
            items.push_back(std::make_shared<earl::value::Str>(entry.path()));
    }
    catch (const std::filesystem::filesystem_error &e) {
        Err::err_wexpr(expr);
        const char *err = e.what();
        std::string msg = "could not list directory `"+path+"`:"+err;
        throw InterpreterException(msg);
    }

    lst->append(items);

    return lst;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_cd(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                         std::shared_ptr<Ctx> &ctx,
                         Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_cd__", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1,
                                  "__internal_cd__", expr);
    const std::string &new_dir = params[0]->to_cxxstring();

    if (chdir(new_dir.c_str()) != 0) {
        Err::err_wexpr(expr);
        const std::string msg = "failed to change directory";
        throw InterpreterException(msg);
    }

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_unix_system__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                               std::shared_ptr<Ctx> &ctx,
                                               Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_unix_system__", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1, "__internal_unix_system__", expr);
    const std::string cmd = params[0]->to_cxxstring();
    int exitcode = system(cmd.c_str());
    if (exitcode == -1 && ((flags & __ERROR_ON_BASH_FAIL) != 0)) {
        Err::err_wexpr(expr);
        const std::string msg = "failed to execute system command `"+cmd+"`";
        throw InterpreterException(msg);
    }
    else if (exitcode == -1)
        WARN_WARGS("failed to execute system command `%s`", expr, cmd.c_str());
    return std::make_shared<earl::value::Int>(exitcode);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_unix_system_woutput__(
    std::vector<std::shared_ptr<earl::value::Obj>> &params,
    std::shared_ptr<Ctx> &ctx, Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_unix_system__woutput_", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1, "__internal_unix_system__woutput_", expr);
    const std::string cmd = params[0]->to_cxxstring();

    std::array<char, 256> buffer;
    std::string output = "";

    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        Err::err_wexpr(expr);
        const std::string msg = "failed to execute system command `" + cmd + "`";
        throw InterpreterException(msg);
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }

    int ec = pclose(pipe);
    if (ec == -1) {
        Err::err_wexpr(expr);
        const std::string msg = "failed to execute system command `" + cmd + "`";
        throw InterpreterException(msg);
    }

    std::vector<std::shared_ptr<earl::value::Obj>> res = {
        std::make_shared<earl::value::Int>(ec),
        std::make_shared<earl::value::Str>(output),
    };
    return std::make_shared<earl::value::Tuple>(std::move(res));
}


std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_type(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "type", expr);
    return std::make_shared<earl::value::Str>(earl::value::type_to_str(params[0]->type()));
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_typeof(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                             std::shared_ptr<Ctx> &ctx,
                             Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "typeof", expr);
    return std::make_shared<earl::value::TypeKW>(params[0]->type());
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_unimplemented(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    std::cout << "[EARL] UNIMPLEMENTED";
    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(params, ctx, expr);
    }
    exit(1);
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_exit(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    if (params.size() == 0)
        exit(0);
    else {
        __INTR_ARGS_MUSTBE_SIZE(params, 1, "exit", expr);
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Int, 1, "exit", expr);
        exit(dynamic_cast<earl::value::Int *>(params[0].get())->value());
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_warn(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(params, "warn", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1, "warn", expr);
    std::cout << "[EARL] WARN: ";
    Intrinsics::intrinsic_println(params, ctx, expr);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_panic(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    std::cout << "[EARL] PANIC";

    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(params, ctx, expr);
    }

    exit(1);

    return nullptr; // unreachable
}

static void
__intrinsic_print(std::shared_ptr<earl::value::Obj> param, std::ostream *stream = nullptr) {
    if (stream == nullptr)
        stream = &std::cout;
    *stream << param->to_cxxstring();
    // Clearing for the REPL
    if ((flags & __REPL) != 0)
        *stream << "\033[K" << std::flush;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_assert(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                             std::shared_ptr<Ctx> &ctx,
                             Expr *expr) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i) {
        earl::value::Obj *param = params.at(i).get();
        if (!param->boolean()) {
            Err::err_wexpr(expr);
            std::string msg = "assertion failure (" + param->to_cxxstring() + ") (with expression_index="+std::to_string(i)+")";
            throw InterpreterException(msg);
        }
    }
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_observe(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                              std::shared_ptr<Ctx> &ctx,
                              Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 2, "observe", expr);
    __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR(params[1],
                                            earl::value::Type::FunctionRef,
                                            earl::value::Type::Closure, 2, "observe", expr);

    auto listen_arg = params.at(0).get();
    auto event = params.back().get();

    earl::variable::event_listener_t callback = {};

    if (event->type() == earl::value::Type::FunctionRef) {
        auto func = dynamic_cast<earl::value::FunctionRef *>(params.back().get())->value();
        if (func->params_len() != 1) {
            Err::err_wexpr(expr);
            const std::string msg = "Observe callback functions must have 1 parameter";
            throw InterpreterException(msg);
        }
        callback = std::dynamic_pointer_cast<earl::value::FunctionRef>(params.back());
    }
    else {
        auto cl = std::dynamic_pointer_cast<earl::value::Closure>(params.back());
        if (cl->params_len() != 1) {
            Err::err_wexpr(expr);
            const std::string msg = "Observe callback functions must have 1 parameter";
            throw InterpreterException(msg);
        }
        callback = cl;
    }

    auto listen_owner = listen_arg->borrow_owner();
    if (!listen_owner) {
        Err::err_wexpr(expr);
        const std::string msg = "cannot set an event listener on a non-variable";
        throw InterpreterException(msg);
    }

    listen_owner->m_event_listener = callback;

    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_init_seed(std::vector<std::shared_ptr<earl::value::Obj>> &seed,
                                std::shared_ptr<Ctx> &ctx,
                                Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(seed, 1, "seed", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT_EXACT(seed[0], earl::value::Type::Int, 1, "seed", expr);
    unsigned s = (unsigned)dynamic_cast<earl::value::Int *>(seed[0].get())->value();
    std::srand(s);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_random(std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                             std::shared_ptr<Ctx> &ctx,
                             Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "rand", expr);
    return std::make_shared<earl::value::Int>(std::rand());
}


std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_print(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i)
        __intrinsic_print(params[i]);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_println(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                              std::shared_ptr<Ctx> &ctx,
                              Expr *expr) {

    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i)
        __intrinsic_print(params[i]);
    std::cout << '\n';
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_fprintln(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                               std::shared_ptr<Ctx> &ctx,
                               Expr *expr) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(params, "fprintln", expr);
    __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR(params[0], earl::value::Type::Int, earl::value::Type::File, 1, "fprintln", expr);
    std::ostream *stream = nullptr;
    if (params[0]->type() == earl::value::Type::Int) {
        auto *st = dynamic_cast<earl::value::Int *>(params[0].get());
        switch (st->value()) {
        case 0:
            assert(false && "unimplemented");
        case 1:
            stream = &std::cout;
            break;
        case 2:
            stream = &std::cerr;
            break;
        default:
            assert(false && "unimplemented");
        }
    }
    else {
        assert(false && "unimplemented");
    }

    for (size_t i = 1; i < params.size(); ++i)
        __intrinsic_print(params[i], stream);
    *stream << '\n';
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_fprint(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                             std::shared_ptr<Ctx> &ctx,
                             Expr *expr) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(params, "fprint", expr);
    __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR(params[0], earl::value::Type::Int, earl::value::Type::File, 1, "fprint", expr);
    std::ostream *stream = nullptr;
    if (params[0]->type() == earl::value::Type::Int) {
        auto *st = dynamic_cast<earl::value::Int *>(params[0].get());
        switch (st->value()) {
        case 0:
            assert(false && "unimplemented");
        case 1:
            stream = &std::cout;
            break;
        case 2:
            stream = &std::cerr;
            break;
        default:
            assert(false && "unimplemented");
        }
    }
    else {
        assert(false && "unimplemented");
    }

    for (size_t i = 1; i < params.size(); ++i)
        __intrinsic_print(params[i], stream);
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_input(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    (void)ctx;
    intrinsic_print(params, ctx, expr);
    std::string in = "";
    std::getline(std::cin, in);
    return std::make_shared<earl::value::Str>(in);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_datetime(std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                               std::shared_ptr<Ctx> &ctx,
                               Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(unused, 0, "datetime", expr);
    return std::make_shared<earl::value::Time>(std::time(nullptr));
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_sleep(std::vector<std::shared_ptr<earl::value::Obj>> &time,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(time, 1, "sleep", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT_EXACT(time[0], earl::value::Type::Int, 1, "sleep", expr);
    usleep(dynamic_cast<earl::value::Int *>(time[0].get())->value());
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_env(std::vector<std::shared_ptr<earl::value::Obj>> &var,
                          std::shared_ptr<Ctx> &ctx, Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(var, 1, "env", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT_EXACT(var[0], earl::value::Type::Str, 1,
                                        "env", expr);
    auto str = dynamic_cast<earl::value::Str *>(var[0].get());
    const char *env = getenv(str->value().c_str());
    return env
        ? std::make_shared<earl::value::Str>(std::string(env))
        : std::make_shared<earl::value::Str>("");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_some(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "some", expr);
    return std::make_shared<earl::value::Option>(params[0]);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_open(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx,
                           Expr *expr) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 2, "open", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1, "open", expr);
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[1], earl::value::Type::Str, 2, "open", expr);

    auto fp = dynamic_cast<earl::value::Str *>(params[0].get());
    auto mode = dynamic_cast<earl::value::Str *>(params[1].get());
    std::fstream stream;
    std::ios_base::openmode om{};

    for (char &c : mode->value()) {
        switch (c) {
        case 'r': om |= std::ios::in; break;
        case 'w': om |= std::ios::out; break;
        case 'b': om |= std::ios::binary; break;
        default: {
            Err::err_wexpr(expr);
            std::string msg = "invalid mode `"+std::to_string(c)+"` for file handler, must be either r|w|b";
            throw InterpreterException(msg);
        } break;
        }
    }

    stream.open(fp->value(), om);

    if (!stream) {
        Err::err_wexpr(expr);
        std::string msg = "file `"+fp->value()+"` could not be found";
        throw InterpreterException(msg);
    }

    auto f = std::make_shared<earl::value::File>(std::dynamic_pointer_cast<earl::value::Str>(params[0]),
                                                 std::dynamic_pointer_cast<earl::value::Str>(params[1]),
                                                 std::move(stream));
    f->set_open();
    return f;
}

