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

#include <stdlib.h>
#include <string.h>

#include "runtime/EARL-object.h"
#include "mem/mem.h"
#include "misc/err.h"
#include "misc/utils.h"

EARL_object_t __earl_object_create(EARL_object_type_t type) {
    return (EARL_object_t) {
        .type = type,
    };
}

void earl_object_destroy(EARL_object_t *obj) {
    switch (obj->type) {
    case EARL_OBJECT_TYPE_STRING: {
        EARL_object_string_t *str = (EARL_object_string_t *)obj;
        free(str->chars);
        str->len = str->cap = 0;
    } break;
    case EARL_OBJECT_TYPE_FUNCTION: {
        EARL_object_function_t *f = (EARL_object_function_t *)obj;
        earl_object_destroy((EARL_object_t *)f->id);
        f->arity = 0;
        memset(f->opcode.data, 0x0, f->opcode.cap);
    } break;
    default: err_wargs("%s: unknown object type `%d`", __FUNCTION__, (int)obj->type);
    }
}
