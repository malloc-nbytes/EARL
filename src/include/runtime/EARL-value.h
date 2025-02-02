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

#include <stdint.h>

#include "parsing/ast.h"
#include "runtime/EARL-value.h"
#include "runtime/builtin-sigs.h"

typedef enum {
    EARL_VALUE_TYPE_UNIT,
    EARL_VALUE_TYPE_INTEGER,
    EARL_VALUE_TYPE_FUNCTION_REFERENCE,
    EARL_VALUE_TYPE_BUILTIN_FUNCTION_REFERENCE,
    EARL_VALUE_TYPE_STRING,
} EARL_value_type_t;

typedef struct EARL_value {
    uint32_t refc;

    EARL_value_type_t type;

    union {
        void *unit;
        int integer;
        stmt_fn_t *fn;
        builtin_f_sig_t builtin_fun;
    } actual;

    to_cstr_sig_t to_cstr;
    boolean_sig_t boolean;
    mutate_sig_t mutate;
} EARL_value_t;

/// @brief Allocate a new EARL value
/// @param type The type of the value
/// @param data The C data
/// @return The allocated EARL value
EARL_value_t *EARL_value_alloc(EARL_value_type_t type, void *data);

/// @brief Debug print an EARL runtime value
/// @param value The value to print
void EARL_value_dump(const EARL_value_t *value);

#endif // EARL_VALUE_H
