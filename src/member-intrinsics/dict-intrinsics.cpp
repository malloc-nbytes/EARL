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

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction>
Intrinsics::intrinsic_dict_member_functions = {
    {"insert", &Intrinsics::intrinsic_member_insert},
};

std::shared_ptr<earl::value::Obj>
Intrinsics::intrinsic_member_insert(std::shared_ptr<earl::value::Obj> obj,
                                    std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                    std::shared_ptr<Ctx> &ctx) {
    __INTR_ARGS_MUSTBE_SIZE(params, 2, "insert");
    switch (obj->type()) {
    case earl::value::Type::DictInt: {
        auto dict = dynamic_cast<earl::value::Dict<int> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert");
        int key = dynamic_cast<earl::value::Int *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    case earl::value::Type::DictStr: {
        auto dict = dynamic_cast<earl::value::Dict<std::string> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert");
        std::string key = dynamic_cast<earl::value::Str *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    case earl::value::Type::DictChar: {
        auto dict = dynamic_cast<earl::value::Dict<char> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert");
        char key = dynamic_cast<earl::value::Char *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    case earl::value::Type::DictFloat: {
        auto dict = dynamic_cast<earl::value::Dict<double> *>(obj.get());
        __INTR_ARG_MUSTBE_TYPE_COMPAT(params[0], dict->ktype(), 1, "insert");
        double key = dynamic_cast<earl::value::Float *>(params[0].get())->value();
        dict->insert(key, params[1]);
    } break;
    default: {
        const std::string &msg = "cannot insert value of type `"
            +earl::value::type_to_str(obj->type())
            +"` as the key being inserted into a dictionary must be the same dictionary key type";
        throw InterpreterException(msg);
    }
    }
    return std::make_shared<earl::value::Void>();
}

