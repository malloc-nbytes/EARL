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

#ifndef EARL_VALUE_H
#define EARL_VALUE_H

#include "builtin-sigs.h"

typedef struct EARL_object         EARL_object_t;
typedef struct EARL_object_string  EARL_object_string_t;

typedef enum EARL_value_type {
    EARL_VALUE_TYPE_UNIT,
    EARL_VALUE_TYPE_INTEGER,
    EARL_VALUE_TYPE_BOOLEAN,
    EARL_VALUE_TYPE_BUILTIN_FUNCTION_REFERENCE,

    __EARL_VALUE_TYPE_LEN, // Used in types.c

    EARL_VALUE_TYPE_OBJECT, // Must be placed *AFTER LEN*
} EARL_value_type_t;

typedef struct EARL_value {
    EARL_value_type_t type;

    union {
        int boolean;
        int integer;
        EARL_object_t *obj;
        builtin_f_sig_t builtin_function_reference;
    } as;

    to_cstr_sig_t to_cstr;
    add_sig_t add;
    sub_sig_t sub;
    mul_sig_t mul;
    div_sig_t div;
    mod_sig_t mod;
    mutate_sig_t mutate;
    is_truthy_sig_t is_truthy;
    eq_sig_t eq;
    neq_sig_t neq;
    cmp_sig_t cmp;
} EARL_value_t;

const char *earl_value_type_to_cstr(const EARL_value_t *const value);

EARL_value_t earl_value_integer_create(int x);
EARL_value_t earl_value_unit_create(void);
EARL_value_t earl_value_boolean_create(int b);
EARL_value_t earl_value_builtin_function_reference_create(builtin_f_sig_t fun);
EARL_value_t earl_value_object_create(EARL_object_t *obj);

// Unsupported Intrinsic Functions
const char * unsupported_to_cstr(const EARL_value_t *const self);
EARL_value_t unsupported_add(const EARL_value_t *const self, const EARL_value_t *const other);
EARL_value_t unsupported_sub(const EARL_value_t *const self, const EARL_value_t *const other);
EARL_value_t unsupported_mul(const EARL_value_t *const self, const EARL_value_t *const other);
EARL_value_t unsupported_div(const EARL_value_t *const self, const EARL_value_t *const other);
EARL_value_t unsupported_mod(const EARL_value_t *const self, const EARL_value_t *const other);
void         unsupported_mutate(EARL_value_t *self, const EARL_value_t *const other);
int          unsupported_is_truthy(const EARL_value_t *const self);
EARL_value_t unsupported_eq(const EARL_value_t *const self, const EARL_value_t *const other);
EARL_value_t unsupported_neq(const EARL_value_t *const self, const EARL_value_t *const other);
EARL_value_t unsupported_cmp(const EARL_value_t *const self, const EARL_value_t *const other, uint8_t opc);

#endif // EARL_VALUE_H
