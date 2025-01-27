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

#include <stdio.h>
#include <stdlib.h>

#include "EARL-value.h"
#include "utils.h"
#include "err.h"

EARL_value_t *EARL_value_alloc(EARL_value_type_t type, void *data) {
    EARL_value_t *v = s_malloc(sizeof(EARL_value_t), NULL, NULL);
    v->type = type;
    switch (type) {
    case EARL_VALUE_TYPE_INTEGER: {
        v->value.integer = *(int *)data;
    } break;
    case EARL_VALUE_TYPE_UNIT: {
        v->value.unit = data;
    } break;
    default:
        err_wargs("unknown type: %d", (int)type);
    }
    return v;
}

void EARL_value_dump(const EARL_value_t *value) {
    switch (value->type) {
    case EARL_VALUE_TYPE_INTEGER:
        printf("Integer: %d\n", value->value.integer);
        break;
    case EARL_VALUE_TYPE_UNIT:
        printf("Unit\n");
        break;
    default:
        err("unknown type");
    }
}