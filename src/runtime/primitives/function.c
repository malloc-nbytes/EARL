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
#include <string.h>
#include <stdio.h>

#include "runtime/primitives/function.h"
#include "runtime/primitives/string.h"
#include "runtime/EARL-object.h"
#include "runtime/EARL-value.h"
#include "runtime/types.h"
#include "mem/mem.h"
#include "misc/utils.h"

EARL_object_function_t *
earl_object_function_alloc(size_t arity, opcode_t opcode, const char *id) {
    EARL_object_function_t *f = (EARL_object_function_t *)mem_s_malloc(sizeof(EARL_object_function_t), NULL, NULL);
    f->base = __earl_object_create(EARL_OBJECT_TYPE_FUNCTION);
    f->arity = arity;
    f->opcode = opcode;
    f->id = earl_object_string_alloc(id);
    return f;
}

EARL_object_function_t *earl_object_function_alloc_empty(void) {
    EARL_object_function_t *f = (EARL_object_function_t *)mem_s_malloc(sizeof(EARL_object_function_t), NULL, NULL);
    f->base = __earl_object_create(EARL_OBJECT_TYPE_FUNCTION);
    f->arity = 0;
    f->opcode = (opcode_t) {
        .data = NULL,
        .len = 0,
        .cap = 0,
    };
    f->id = NULL;
    return f;
}

const char *earl_object_function_to_cstr(const EARL_value_t *const value) {
    return "<Function>";
}
