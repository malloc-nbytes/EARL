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
#include <iostream>

#include "earl.hpp"
#include "token.hpp"
#include "common.hpp"
#include "err.hpp"
#include "utils.hpp"

static const std::unordered_map<earl::value::Type, std::vector<earl::value::Type>> type_map = {
    {earl::value::Type::Int, {
            earl::value::Type::Int,
            earl::value::Type::Float,
        }
    },
    {earl::value::Type::Float, {
            earl::value::Type::Float,
            earl::value::Type::Int,
        }
    },
    {earl::value::Type::Bool, {
            earl::value::Type::Bool,
        }
    },
    {earl::value::Type::Char, {
            earl::value::Type::Char,
            earl::value::Type::Str,
        }
    },
    {earl::value::Type::Str, {
            earl::value::Type::Str,
            earl::value::Type::Char,
        }
    },
    {earl::value::Type::List, {
            earl::value::Type::List,
        }
    },
    {earl::value::Type::Void, {
            earl::value::Type::Void,
        }
    },
    {earl::value::Type::Option, {
            earl::value::Type::Option,
        }
    },
    {earl::value::Type::Tuple, {
            earl::value::Type::Tuple,
        }
    },
    {earl::value::Type::Slice, {
            earl::value::Type::Slice,
        }
    },
    {earl::value::Type::TypeKW, {
            earl::value::Type::TypeKW,
        }
    },
    {earl::value::Type::Time, {
            earl::value::Type::Time,
        }
    },
    {earl::value::Type::FunctionRef, {
            earl::value::Type::FunctionRef,
        }
    },
    {earl::value::Type::ClassRef, {
            earl::value::Type::ClassRef,
        }
    },
    {earl::value::Type::Closure, {
            earl::value::Type::Closure,
        }
    },
    {earl::value::Type::Predicate, {
            earl::value::Type::Predicate,
        }
    },
};

std::string earl::value::type_to_str(earl::value::Type ty) {
    switch (ty) {
    case earl::value::Type::Int:         return "int";
    case earl::value::Type::Float:       return "float";
    case earl::value::Type::Bool:        return "bool";
    case earl::value::Type::Char:        return "char";
    case earl::value::Type::Str:         return "str";
    case earl::value::Type::List:        return "list";
    case earl::value::Type::Void:        return "unit";
    case earl::value::Type::Option:      return "option";
    case earl::value::Type::Tuple:       return "tuple";
    case earl::value::Type::Class:       return "class";
    case earl::value::Type::Module:      return "module";
    case earl::value::Type::Enum:        return "enum";
    case earl::value::Type::File:        return "file";
    case earl::value::Type::Slice:       return "slice";
    case earl::value::Type::Closure:     return "closure";
    case earl::value::Type::TypeKW:      return "TypeKW";
    case earl::value::Type::DictInt:     return "DictInt";
    case earl::value::Type::DictStr:     return "DictStr";
    case earl::value::Type::DictChar:    return "DictChar";
    case earl::value::Type::DictFloat:   return "DictFloat";
    case earl::value::Type::Time:        return "time";
    case earl::value::Type::Return:      return "unit";
    case earl::value::Type::FunctionRef: return "FunctionRef";
    case earl::value::Type::ClassRef:    return "ClassRef";
    case earl::value::Type::Predicate:   return "Predicate";
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
            if (ty2 == compatible_type)
                return true;
        }
    }

    return false;
}

