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

using namespace earl::value;

static const std::unordered_map<std::string, earl::value::Type> str_to_type_map = {
    {COMMON_EARLTY_INT32, earl::value::Type::Int},
    {COMMON_EARLTY_STR, earl::value::Type::Str},
    {COMMON_EARLTY_UNIT, earl::value::Type::Void},
};

bool earl::value::type_is_compatable(Obj *const obj1, Obj *const obj2) {

    // They are lists, make sure the lists hold the same types.
    if (obj1->type() == Type::List && obj2->type() == Type::List) {
        // List *const list1 = dynamic_cast<List *const>(obj1);
        // List *const list2 = dynamic_cast<List *const>(obj2);

        // if (list1->value().size() != list2->value().size()) {
        //     return false;
        // }

        // for (size_t i = 0; i < list1->value().size(); ++i) {
        //     if (!type_is_compatable(list1->value()[i], list2->value()[i])) {
        //         return false;
        //     }
        // }
        // TODO("earl::value::type_is_compatable: List type compatibility");
        return true;
    }

    // Not list type, just check the primitive type
    if (obj1->type() == obj2->type()) {
        return true;
    }
    return false;
}

