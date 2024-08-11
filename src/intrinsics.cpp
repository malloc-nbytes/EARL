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

const std::unordered_map<std::string, Intrinsics::IntrinsicFunction>
Intrinsics::intrinsic_functions = {
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
    {"fprintln", &Intrinsics::intrinsic_fprintln},
    {"fprint", &Intrinsics::intrinsic_fprint},
    // Casting Functions
    {"str", &Intrinsics::intrinsic_str},
    {"int", &Intrinsics::intrinsic_int},
    {"float", &Intrinsics::intrinsic_float},
    {"bool", &Intrinsics::intrinsic_bool},
};

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_member_functions = {
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"back", &Intrinsics::intrinsic_member_back},
    {"filter", &Intrinsics::intrinsic_member_filter},
    {"foreach", &Intrinsics::intrinsic_member_foreach},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
    {"pop", &Intrinsics::intrinsic_member_pop},
    {"split", &Intrinsics::intrinsic_member_split},
    {"substr", &Intrinsics::intrinsic_member_substr},
    {"trim", &Intrinsics::intrinsic_member_trim},
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

std::shared_ptr<earl::value::Obj>
Intrinsics::call(const std::string &id,
                 std::vector<std::shared_ptr<earl::value::Obj>> &params,
                 std::shared_ptr<Ctx> &ctx) {
    return intrinsic_functions.at(id)(params, ctx);
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
    case earl::value::Type::Int: return false;
    case earl::value::Type::Char: return Intrinsics::intrinsic_char_member_functions.find(id) != Intrinsics::intrinsic_char_member_functions.end();
    case earl::value::Type::Str: return Intrinsics::intrinsic_str_member_functions.find(id) != Intrinsics::intrinsic_str_member_functions.end();
    case earl::value::Type::Bool: return false;
    case earl::value::Type::List: return Intrinsics::intrinsic_list_member_functions.find(id) != Intrinsics::intrinsic_list_member_functions.end();
    case earl::value::Type::Option: return Intrinsics::intrinsic_option_member_functions.find(id) != Intrinsics::intrinsic_option_member_functions.end();
    case earl::value::Type::File: return Intrinsics::intrinsic_file_member_functions.find(id) != Intrinsics::intrinsic_file_member_functions.end();
    case earl::value::Type::Tuple: return Intrinsics::intrinsic_tuple_member_functions.find(id) != Intrinsics::intrinsic_tuple_member_functions.end();
    default: return false;
    }
    return Intrinsics::intrinsic_member_functions.find(id) != Intrinsics::intrinsic_member_functions.end();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::call_member(const std::string &id,
                        earl::value::Type type,
                        std::shared_ptr<earl::value::Obj> accessor,
                        std::vector<std::shared_ptr<earl::value::Obj>> &params,
                        std::shared_ptr<Ctx> &ctx) {
    switch (type) {
    case earl::value::Type::Int: assert(false);
    case earl::value::Type::Char: return Intrinsics::intrinsic_char_member_functions.at(id)(accessor, params, ctx);
    case earl::value::Type::Str: return Intrinsics::intrinsic_str_member_functions.at(id)(accessor, params, ctx);
    case earl::value::Type::Bool: assert(false);
    case earl::value::Type::List: return Intrinsics::intrinsic_list_member_functions.at(id)(accessor, params, ctx);
    case earl::value::Type::Option: return Intrinsics::intrinsic_option_member_functions.at(id)(accessor, params, ctx);
    case earl::value::Type::File: return Intrinsics::intrinsic_file_member_functions.at(id)(accessor, params, ctx);
    case earl::value::Type::Tuple: return Intrinsics::intrinsic_tuple_member_functions.at(id)(accessor, params, ctx);
    default: assert(false);
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_str(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "str");
    return std::make_shared<earl::value::Str>(params[0]->to_cxxstring());
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_int(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "int");
    {
        std::vector<earl::value::Type> tys = {
            earl::value::Type::Int,
            earl::value::Type::Float,
            earl::value::Type::Str,
        };
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], tys, 1, "int");
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
    default: {
    } break;
        ERR_WARGS(Err::Type::Fatal, "cannot convert type `%s` to type float",
                  earl::value::type_to_str(params[0]->type()).c_str());
    }
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_float(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "float");
    {
        std::vector<earl::value::Type> tys = {
            earl::value::Type::Int,
            earl::value::Type::Float,
            earl::value::Type::Str,
        };
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], tys, 1, "float");
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
        ERR_WARGS(Err::Type::Fatal, "cannot convert type `%s` to type float",
                  earl::value::type_to_str(params[0]->type()).c_str());
    } break;
    }
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_bool(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "bool");
    {
        std::vector<earl::value::Type> tys = {
            earl::value::Type::Int,
            earl::value::Type::Float,
            earl::value::Type::Str,
        };
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], tys, 1, "bool");
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
        ERR_WARGS(Err::Type::Fatal, "cannot convert str `%s` to type bool", s.c_str());
    } break;
    default: {
        ERR_WARGS(Err::Type::Fatal, "cannot convert type `%s` to type float",
                  earl::value::type_to_str(params[0]->type()).c_str());
    } break;
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_len(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                          std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "len");
    {
        std::vector<earl::value::Type> lst = {earl::value::Type::List, earl::value::Type::Str, earl::value::Type::Tuple};
        __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(params[0], lst, 1, "len");
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
Intrinsics::intrinsic_argv(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 0, "argv");
    std::vector<std::shared_ptr<earl::value::Obj>> args = {};
    for (size_t i = 0; i < earl_argv.size(); ++i)
        args.push_back(std::make_shared<earl::value::Str>(earl_argv.at(i)));
    return std::make_shared<earl::value::List>(args);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_move__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                        std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    (void)params;
    UNIMPLEMENTED("Intrinsics::intrinsic___internal_move__");
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_mkdir__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                         std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_mkdir__");
    auto obj = params[0];
    std::string path = obj->to_cxxstring();
    if (!std::filesystem::exists(path))
        if (!std::filesystem::create_directory(path))
            ERR_WARGS(Err::Type::Fatal, "could not create directory `%s`", path.c_str());
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic___internal_ls__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                      std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "__internal_ls__");

    auto obj = params[0];
    std::string path = obj->to_cxxstring();

    auto lst = std::make_shared<earl::value::List>();
    std::vector<std::shared_ptr<earl::value::Obj>> items={};

    try {
        for (const auto &entry : std::filesystem::directory_iterator(path))
            items.push_back(std::make_shared<earl::value::Str>(entry.path()));
    }
    catch (const std::filesystem::filesystem_error &e) {
        const char *err = e.what();
        ERR_WARGS(Err::Type::Fatal, "could not list directory `%s`: %s", path.c_str(), err);
    }

    lst->append(items);

    return lst;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_type(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "type");
    return std::make_shared<earl::value::Str>(earl::value::type_to_str(params[0]->type()));
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_unimplemented(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                    std::shared_ptr<Ctx> &ctx) {
    std::cout << "[EARL] UNIMPLEMENTED";
    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(params, ctx);
    }
    exit(1);
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_exit(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Int, 1, "exit");
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "exit");
    exit(dynamic_cast<earl::value::Int *>(params[0].get())->value());
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_panic(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx) {
    std::cout << "[EARL] PANIC";

    if (params.size() != 0) {
        std::cout << ": ";
        Intrinsics::intrinsic_println(params, ctx);
    }

    exit(1);

    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_assert(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                             std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i) {
        earl::value::Obj *param = params.at(i).get();
        if (!param->boolean()) {
            ERR_WARGS(Err::Type::Assertion,
                      "assertion failure (expression=%zu) (earl::value::Type=%d)",
                      i+1, (int)param->type());
        }
    }
    return nullptr;
}

static void
__intrinsic_print(std::shared_ptr<earl::value::Obj> param, std::ostream *stream = nullptr) {
    if (stream == nullptr)
        stream = &std::cout;
    switch (param->type()) {
    case earl::value::Type::Int: {
        auto *intparam = dynamic_cast<earl::value::Int *>(param.get());
        *stream << intparam->value();
    } break;
    case earl::value::Type::Float: {
        auto *floatparam = dynamic_cast<earl::value::Float *>(param.get());
        *stream << floatparam->value();
    } break;
    case earl::value::Type::Bool: {
        auto *bool_param = dynamic_cast<earl::value::Bool *>(param.get());
        *stream << (bool_param->value() ? "true" : "false");
    } break;
    case earl::value::Type::Option: {
        auto *option_param = dynamic_cast<earl::value::Option *>(param.get());

        if (option_param->is_none())
            *stream << "<none>";
        else {
            *stream << "some(";
            __intrinsic_print(option_param->value());
            *stream << ")";
        }

    } break;
    case earl::value::Type::Str: {
        auto *strparam = dynamic_cast<earl::value::Str *>(param.get());
        *stream << strparam->value();
    } break;
    case earl::value::Type::Char: {
        auto *strparam = dynamic_cast<earl::value::Char *>(param.get());
        *stream << strparam->value();
    } break;
    case earl::value::Type::List: {
        earl::value::List *listparam = dynamic_cast<earl::value::List *>(param.get());
        *stream << '[';
        std::vector<std::shared_ptr<earl::value::Obj>> &list = listparam->value();

        for (size_t i = 0; i < list.size(); ++i) {
            __intrinsic_print(list[i]);
            if (i != list.size()-1)
                *stream << ", ";
        }

        *stream << ']';
    } break;
    case earl::value::Type::Tuple: {
        earl::value::Tuple *tupleparam = dynamic_cast<earl::value::Tuple *>(param.get());
        *stream << '(';
        auto values = tupleparam->value();
        for (size_t i = 0; i < values.size(); ++i) {
            __intrinsic_print(values[i]);
            if (i != values.size()-1)
                *stream << ", ";
        }
        *stream << ')';
    } break;
    case earl::value::Type::Class: {
        auto *classparam = dynamic_cast<earl::value::Class *>(param.get());
        *stream << "<Class " << classparam->id() << " { ";

        auto class_ctx = dynamic_cast<ClassCtx *>(classparam->ctx().get());
        auto members = class_ctx->get_printable_members();

        for (size_t i = 0; i < members.size(); ++i) {
            auto &mem = members[i];
            *stream << mem->id() << " = ";
            __intrinsic_print(mem->value(), stream);
            if (i != members.size()-1)
                *stream << ", ";
        }

        *stream << " }>";
    } break;
    case earl::value::Type::Closure: {
        auto *closureparam = dynamic_cast<earl::value::Closure *>(param.get());
        *stream << "<closure>";
    } break;
    case earl::value::Type::Enum: {
        auto *enumparam = dynamic_cast<earl::value::Enum *>(param.get());
        *stream << "<Enum " << enumparam->id() << " { ";
        auto &elems = enumparam->extract();
        size_t i = 0;
        for (auto &it : elems) {
            *stream << it.first << " = ";
            __intrinsic_print(it.second->value(), stream);
            if (i != elems.size()-1)
                *stream << ", ";
            ++i;
        }
        *stream << " }>";
    } break;
    default: {
        ERR_WARGS(Err::Type::Fatal, "intrinsic_print: unknown parameter type %d", static_cast<int>(param->type()));
    } break;
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_print(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i)
        __intrinsic_print(params[i]);
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_println(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                              std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i)
        __intrinsic_print(params[i]);
    std::cout << '\n';
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_fprintln(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                               std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(params, "fprintln");
    __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR(params[0], earl::value::Type::Int, earl::value::Type::File, 1, "fprintln");
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
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_fprint(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                             std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __MEMBER_INTR_ARGS_MUSTNOT_BE_0(params, "fprintln");
    __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR(params[0], earl::value::Type::Int, earl::value::Type::File, 1, "fprintln");
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
    return nullptr;
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_input(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    intrinsic_print(params, ctx);
    std::string in = "";
    std::getline(std::cin, in);
    return std::make_shared<earl::value::Str>(in);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_some(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 1, "some");
    if (params.size() != 1)
        ERR_WARGS(Err::Type::Fatal, "`some` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    return std::make_shared<earl::value::Option>(params[0]);
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_open(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                           std::shared_ptr<Ctx> &ctx) {
    (void)ctx;
    __INTR_ARGS_MUSTBE_SIZE(params, 2, "open");
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], earl::value::Type::Str, 1, "open");
    __INTR_ARG_MUSTBE_TYPE_COMPAT(params[1], earl::value::Type::Str, 2, "open");

    auto fp = dynamic_cast<earl::value::Str *>(params[0].get());
    auto mode = dynamic_cast<earl::value::Str *>(params[1].get());
    std::fstream stream;
    std::ios_base::openmode om{};

    for (char &c : mode->value()) {
        switch (c) {
        case 'r': om |= std::ios::in; break;
        case 'w': om |= std::ios::out; break;
        case 'b': om |= std::ios::binary; break;
        default: ERR_WARGS(Err::Type::Fatal, "invalid mode `%c` for file handler, must be either r|w|b", c);
        }
    }

    stream.open(fp->value(), om);

    if (!stream)
        ERR_WARGS(Err::Type::Fatal, "file `%s` could not be found", fp->value().c_str());

    auto f = std::make_shared<earl::value::File>(std::dynamic_pointer_cast<earl::value::Str>(params[0]),
                                                 std::dynamic_pointer_cast<earl::value::Str>(params[1]),
                                                 std::move(stream));
    f->set_open();
    return f;
}

