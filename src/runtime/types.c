/** file */

// MIT License

// Copyright (c) 2025 malloc-nbytes

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

#include "runtime/types.h"
#include "runtime/EARL-value.h"
#include "runtime/EARL-object.h"
#include "misc/utils.h"

// Value types
static EARL_value_type_t types_simple_compat_types_unit[]    = {EARL_VALUE_TYPE_UNIT};
static EARL_value_type_t types_simple_compat_types_integer[] = {EARL_VALUE_TYPE_INTEGER};
static EARL_value_type_t types_simple_compat_types_boolean[] = {EARL_VALUE_TYPE_BOOLEAN};
static EARL_value_type_t types_simple_compat_types_builtin_function_reference[] =
    {EARL_VALUE_TYPE_BUILTIN_FUNCTION_REFERENCE};

static EARL_value_type_t *types_simple_compat_types[] = {
    types_simple_compat_types_unit,
    types_simple_compat_types_integer,
    types_simple_compat_types_boolean,
    types_simple_compat_types_builtin_function_reference,
};

// Object types
static EARL_object_type_t types_object_compat_types_string[] = {EARL_OBJECT_TYPE_STRING};

static EARL_object_type_t *types_object_compat_types[] = {
    types_object_compat_types_string,
};

int types_are_compatible(EARL_value_type_t ty1, EARL_value_type_t ty2) {
    _Static_assert(sizeof(types_simple_compat_types)/sizeof(*types_simple_compat_types) == __EARL_VALUE_TYPE_LEN,
                   "EARL value compatible types is incomplete");
    _Static_assert(sizeof(types_object_compat_types)/sizeof(*types_object_compat_types) == __EARL_OBJECT_TYPE_LEN,
                   "EARL object compatible types is incomplete");

    if (ty1 == EARL_VALUE_TYPE_OBJECT) {
        TODO; // Pass object directly for type pruning for inner types
        EARL_object_type_t *types = types_object_compat_types[ty1];
        for (size_t i = 0; i < sizeof(types)/sizeof(*types); ++i)
            if (types[i] == ty2)
                return 1;
    } else {
        EARL_value_type_t *types = types_simple_compat_types[ty1];
        for (size_t i = 0; i < sizeof(types)/sizeof(*types); ++i)
            if (types[i] == ty2)
                return 1;
    }
    return 0;
}
