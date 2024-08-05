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

std::string earl::value::type_to_str(earl::value::Type ty) {
    switch (ty) {
    case earl::value::Type::Int: return "int";
    case earl::value::Type::Bool: return "bool";
    case earl::value::Type::Char: return "char";
    case earl::value::Type::Str: return "str";
    case earl::value::Type::List: return "list";
    case earl::value::Type::Void: return "void";
    case earl::value::Type::Option: return "option";
    default: ERR_WARGS(Err::Type::Fatal, "unknown type of id (%d) in processing", (int)ty);
    }
}

const char *earl::value::type_to_cstr(earl::value::Type ty) {
    return type_to_str(ty).c_str();
}

bool earl::value::type_is_compatable(const earl::value::Obj *const obj1, const earl::value::Obj *const obj2) {
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

bool earl::value::type_is_compatable(earl::value::Type ty1, earl::value::Type ty2) {
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

