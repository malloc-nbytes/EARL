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

#include "earl.hpp"
#include "token.hpp"
#include "common.hpp"
#include "err.hpp"
#include "utils.hpp"

static const std::unordered_map<earl::value::Type, std::vector<earl::value::Type>> type_map = {
    {earl::value::Type::Int, {earl::value::Type::Int}},
    {earl::value::Type::Bool, {earl::value::Type::Bool}},
    {earl::value::Type::Char, {earl::value::Type::Char}},
    {earl::value::Type::Str, {earl::value::Type::Str, earl::value::Type::Char}},
    {earl::value::Type::List, {earl::value::Type::List}},
    {earl::value::Type::Void, {earl::value::Type::Void}},
    {earl::value::Type::Option, {earl::value::Type::Option}},
};

std::string earl::value::type_to_str(earl::value::Obj *obj) {
    switch (obj->type()) {
    case earl::value::Type::Int: return COMMON_EARLTY_INT32;
    case earl::value::Type::Bool: return COMMON_EARLTY_BOOL;
    case earl::value::Type::Char: return COMMON_EARLTY_CHAR;
    case earl::value::Type::Str: return COMMON_EARLTY_STR;
    case earl::value::Type::List: return COMMON_EARLTY_LIST;
    case earl::value::Type::Class: return COMMON_EARLKW_CLASS;
    case earl::value::Type::File: return COMMON_EARLTY_FILE;
    case earl::value::Type::Closure: return COMMON_EARLTY_CLOSURE;
    case earl::value::Type::Option: {
        std::string res = "optional<";
        auto *option = dynamic_cast<earl::value::Option *>(obj);
        if (option->value()) {
            res += type_to_str(option->value());
        }
        return res + ">";
    } break;
    default: ERR_WARGS(Err::Type::Fatal, "unknown type `%d`", (int)obj->type());
    }
}

bool earl::value::type_is_compatable(Obj *const obj1, Obj *const obj2) {
    earl::value::Type ty1 = obj1->type();
    earl::value::Type ty2 = obj2->type();

    if (ty1 == ty2)
        return true;

    auto it = type_map.find(ty1);
    if (it != type_map.end()) {
        const std::vector<earl::value::Type>& compatible_types = it->second;

        for (auto compatible_type : compatible_types) {
            if (ty2 == compatible_type) {
                return true;
            }
        }
    }

    return false;
}

