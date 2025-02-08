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

#include <assert.h>
#include <stdlib.h>

#include "runtime/EARL-value.h"
#include "runtime/EARL-object.h"
#include "runtime/primitives/string.h"
#include "runtime/primitives/integer.h"
#include "runtime/primitives/unit.h"
#include "runtime/primitives/builtin-function-reference.h"
#include "misc/err.h"
#include "misc/utils.h"

EARL_value_t
earl_value_builtin_function_reference_create(builtin_f_sig_t fun) {
    return (EARL_value_t) {
        .type = EARL_VALUE_TYPE_BUILTIN_FUNCTION_REFERENCE,
        .as = { .builtin_function_reference = fun },
        .to_cstr   = earl_value_builtin_function_reference_to_cstr,
        .add       = unsupported_add,
        .sub       = unsupported_sub,
        .mul       = unsupported_mul,
        .div       = unsupported_div,
        .mod       = unsupported_mod,
        .mutate    = unsupported_mutate,
        .is_truthy = unsupported_is_truthy,
    };
}

EARL_value_t earl_value_integer_create(int x) {
    return (EARL_value_t) {
        .type = EARL_VALUE_TYPE_INTEGER,
        .as = { .integer = x },
        .to_cstr   = earl_value_integer_to_cstr,
        .add       = earl_value_integer_add,
        .sub       = earl_value_integer_sub,
        .mul       = earl_value_integer_mul,
        .div       = earl_value_integer_div,
        .mod       = earl_value_mod,
        .mutate    = earl_value_integer_mutate,
        .is_truthy = earl_value_integer_is_truthy,
    };
}

EARL_value_t earl_value_unit_create(void) {
    return (EARL_value_t) {
        .type = EARL_VALUE_TYPE_UNIT,
        .as = { .integer = 0 },
        .to_cstr   = earl_value_unit_to_cstr,
        .add       = unsupported_add,
        .sub       = unsupported_sub,
        .mul       = unsupported_mul,
        .div       = unsupported_div,
        .mod       = unsupported_mod,
        .mutate    = unsupported_mutate,
        .is_truthy = unsupported_is_truthy,
    };
}

EARL_value_t earl_value_boolean_create(int b) {
    return (EARL_value_t) {
        .type = EARL_VALUE_TYPE_BOOLEAN,
        .as = { .boolean = b },
        .to_cstr   = unsupported_to_cstr,
        .add       = unsupported_add,
        .sub       = unsupported_sub,
        .mul       = unsupported_mul,
        .div       = unsupported_div,
        .mod       = unsupported_mod,
        .mutate    = unsupported_mutate,
        .is_truthy = unsupported_is_truthy,
    };
    TODO;
}

EARL_value_t earl_value_object_create(EARL_object_t *obj) {
    EARL_value_t value = (EARL_value_t) {
        .type = EARL_VALUE_TYPE_OBJECT,
        .as = { .obj = obj },
        .to_cstr   = unsupported_to_cstr,
        .add       = unsupported_add,
        .sub       = unsupported_sub,
        .mul       = unsupported_mul,
        .div       = unsupported_div,
        .mod       = unsupported_mod,
        .mutate    = unsupported_mutate,
        .is_truthy = unsupported_is_truthy,
    };

    switch (obj->type) {
    case EARL_OBJECT_TYPE_STRING: {
        value.to_cstr = earl_object_string_to_cstr;
        value.add     = earl_object_string_add;
    } break;
    default: {
        err_wargs("%s: unhandled type: %d\n", __FUNCTION__, obj->type);
    } break;
    }

    return value;
}

const char *unsupported_to_cstr(const EARL_value_t *const self) {
    err_wargs("to_cstr is unsupported for type `%d`", self->type);
}

EARL_value_t unsupported_add(const EARL_value_t *const self, const EARL_value_t *const other) {
    err_wargs("add is unsupported for type `%d`", self->type);
}

EARL_value_t unsupported_sub(const EARL_value_t *const self, const EARL_value_t *const other) {
    err_wargs("sub is unsupported for type `%d`", self->type);
}

EARL_value_t unsupported_mul(const EARL_value_t *const self, const EARL_value_t *const other) {
    err_wargs("mul is unsupported for type `%d`", self->type);
}

EARL_value_t unsupported_div(const EARL_value_t *const self, const EARL_value_t *const other) {
    err_wargs("div is unsupported for type `%d`", self->type);
}

EARL_value_t unsupported_mod(const EARL_value_t *const self, const EARL_value_t *const other) {
    err_wargs("mod is unsupported for type `%d`", self->type);
}

int unsupported_is_truthy(const EARL_value_t *const self) {
    err_wargs("is_truthy is unsupported for type `%d`", self->type);
}

void unsupported_mutate(EARL_value_t *self, const EARL_value_t *const other) {
    err_wargs("mutate is unsupported for type `%d`", self->type);
}
