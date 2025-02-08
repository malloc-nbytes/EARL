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

#include "runtime/EARL-object.h"
#include "runtime/types.h"
#include "mem/mem.h"
#include "misc/utils.h"

EARL_object_string_t *earl_object_string_alloc(const char *chars) {
    EARL_object_string_t *s = (EARL_object_string_t *)mem_s_malloc(sizeof(EARL_object_string_t), NULL, NULL);

    s->base  = __earl_object_create(EARL_OBJECT_TYPE_STRING);
    s->chars = (char *)mem_s_malloc(1, NULL, NULL);
    s->cap   = 1;
    s->len   = 0;

    for (size_t i = 0; chars[i]; ++i)
        da_append(s->chars, s->len, s->cap, char *, chars[i]);

    s->chars[s->len] = '\0';

    return s;
}

const char *earl_object_string_to_cstr(const EARL_value_t *const self) {
    return ((EARL_object_string_t *)self->as.obj)->chars;
}

// TODO: Assert that it is binop compatible.
EARL_value_t earl_object_string_add(const EARL_value_t *self,
                                    const EARL_value_t *const other) {

    EARL_object_string_t *s1     = (EARL_object_string_t *)self->as.obj;
    EARL_object_string_t *s2     = (EARL_object_string_t *)other->as.obj;
    char                 *concat = (char *)mem_s_malloc(s1->len + s2->len + 1, NULL, NULL);

    strcpy(concat, s1->chars);
    strcat(concat, s2->chars);

    EARL_object_string_t *obj = earl_object_string_alloc(concat);
    EARL_value_t res = earl_value_object_create((EARL_object_t *)obj);

    free(concat);

    return res;
}
