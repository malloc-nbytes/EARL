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

#ifndef EARL_OBJECT_H
#define EARL_OBJECT_H

#include <stddef.h>

#include "runtime/EARL-value.h"
#include "runtime/VM/opcode.h"

typedef enum {
    EARL_OBJECT_TYPE_STRING = 0,
    EARL_OBJECT_TYPE_FUNCTION,

    __EARL_OBJECT_TYPE_LEN, // Used in types.c
} EARL_object_type_t;

typedef struct EARL_object {
    EARL_object_type_t type;
} EARL_object_t;

typedef struct EARL_object_string {
    EARL_object_t base;
    char *chars;
    size_t len;
    size_t cap;
} EARL_object_string_t;

typedef struct EARL_object_function {
    EARL_object_t base;
    size_t arity;
    opcode_t opcode;
    EARL_object_string_t *id;
} EARL_object_function_t;

EARL_object_t __earl_object_create(EARL_object_type_t type);
void earl_object_destroy(EARL_object_t *obj);

#endif // EARL_OBJECT_H
