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

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_dict_member_functions = {
    {"insert", &Intrinsics::intrinsic_member_insert},
    {"has_key", &Intrinsics::intrinsic_member_has_key},
    {"has_value", &Intrinsics::intrinsic_member_has_value},
    {"empty", &Intrinsics::intrinsic_member_empty},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_insert(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                    std::shared_ptr<Ctx> &ctx,
                                    Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(params, 2, "insert", expr);
    switch (obj->type()) {
    case earl::value::Type::DictInt: {
        auto dict = dynamic_cast<earl::value::Dict<int> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert", expr);
        int key = dynamic_cast<earl::value::Int *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    case earl::value::Type::DictStr: {
        auto dict = dynamic_cast<earl::value::Dict<std::string> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert", expr);
        std::string key = dynamic_cast<earl::value::Str *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    case earl::value::Type::DictChar: {
        auto dict = dynamic_cast<earl::value::Dict<char> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert", expr);
        char key = dynamic_cast<earl::value::Char *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    case earl::value::Type::DictFloat: {
        auto dict = dynamic_cast<earl::value::Dict<double> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert", expr);
        double key = dynamic_cast<earl::value::Float *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    default: {
        Err::err_wexpr(expr);
        const std::string &msg = "cannot insert value of type `"
            +earl::value::type_to_str(obj->type())
            +"` as the key being inserted into a dictionary must be the same dictionary key type";
        throw InterpreterException(msg);
    }
    }
    return std::make_shared<earl::value::Void>();
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_has_key(std::shared_ptr<earl::value::Obj> obj,
                                     std::vector<std::shared_ptr<earl::value::Obj>> &key,
                                     std::shared_ptr<Ctx> &ctx,
                                     Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(key, 1, "has_key", expr);
    switch (obj->type()) {
    case earl::value::Type::DictInt: {
        auto dict = dynamic_cast<earl::value::Dict<int> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(key[0], dict->ktype(), 1, "has_key", expr);
        int k = dynamic_cast<earl::value::Int *>(key[0].get())->value();
        return std::make_shared<earl::value::Bool>(dict->has_key(k));
    } break;
    case earl::value::Type::DictStr: {
        auto dict = dynamic_cast<earl::value::Dict<std::string> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(key[0], dict->ktype(), 1, "has_key", expr);
        std::string k = dynamic_cast<earl::value::Str *>(key[0].get())->value();
        return std::make_shared<earl::value::Bool>(dict->has_key(k));
    } break;
    case earl::value::Type::DictChar: {
        auto dict = dynamic_cast<earl::value::Dict<char> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(key[0], dict->ktype(), 1, "has_key", expr);
        char k = dynamic_cast<earl::value::Char *>(key[0].get())->value();
        return std::make_shared<earl::value::Bool>(dict->has_key(k));
    } break;
    case earl::value::Type::DictFloat: {
        auto dict = dynamic_cast<earl::value::Dict<double> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(key[0], dict->ktype(), 1, "has_key", expr);
        double k = dynamic_cast<earl::value::Float *>(key[0].get())->value();
        return std::make_shared<earl::value::Bool>(dict->has_key(k));
    } break;
    default: {
        Err::err_wexpr(expr);
        const std::string &msg = "cannot check if a key exists in a dictionary of type `"
            +earl::value::type_to_str(obj->type())
            +"` as the key being checked must be the same dictionary key type";
        throw InterpreterException(msg);
    }
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_has_value(std::shared_ptr<earl::value::Obj> obj,
                            std::vector<std::shared_ptr<earl::value::Obj>> &value,
                            std::shared_ptr<Ctx> &ctx,
                            Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(value, 1, "has_value", expr);
    switch (obj->type()) {
    case earl::value::Type::DictInt: {
        auto dict = dynamic_cast<earl::value::Dict<int> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->has_value(value[0].get()));
    } break;
    case earl::value::Type::DictStr: {
        auto dict = dynamic_cast<earl::value::Dict<std::string> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->has_value(value[0].get()));
    } break;
    case earl::value::Type::DictChar: {
        auto dict = dynamic_cast<earl::value::Dict<char> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->has_value(value[0].get()));
    } break;
    case earl::value::Type::DictFloat: {
        auto dict = dynamic_cast<earl::value::Dict<double> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->has_value(value[0].get()));
    } break;
    default: {
        Err::err_wexpr(expr);
        const std::string &msg = "cannot check if a value exists in a dictionary of type `"
            +earl::value::type_to_str(obj->type())
            +"` as the value being checked must be the same dictionary value type";
        throw InterpreterException(msg);
    }
    }
}

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_empty(std::shared_ptr<earl::value::Obj> obj,
                                   std::vector<std::shared_ptr<earl::value::Obj>> &value,
                                   std::shared_ptr<Ctx> &ctx,
                                   Expr *expr) {
    __INTR_ARGS_MUSTBE_SIZE(value, 0, "empty", expr);
    switch (obj->type()) {
    case earl::value::Type::DictInt: {
        auto dict = dynamic_cast<earl::value::Dict<int> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->empty());
    } break;
    case earl::value::Type::DictStr: {
        auto dict = dynamic_cast<earl::value::Dict<std::string> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->empty());
    } break;
    case earl::value::Type::DictChar: {
        auto dict = dynamic_cast<earl::value::Dict<char> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->empty());
    } break;
    case earl::value::Type::DictFloat: {
        auto dict = dynamic_cast<earl::value::Dict<double> *>(obj.get());
        return std::make_shared<earl::value::Bool>(dict->empty());
    } break;
    default: {
        Err::err_wexpr(expr);
        const std::string &msg = "cannot check if dict type is empty `" +earl::value::type_to_str(obj->type()) +"`";
        throw InterpreterException(msg);
    }
    }
}

