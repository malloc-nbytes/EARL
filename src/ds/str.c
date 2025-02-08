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

#include <string.h>

#include "ds/str.h"
#include "misc/utils.h"
#include "mem/mem.h"

void str_append(str_t *s, char c) {
    da_append(s->data, s->len, s->cap, char *, c);
    s->data[s->len] = '\0';
}

void str_concat_from_cstr(str_t *s, const char *other) {
    for (size_t i = 0; other[i]; ++i)
        str_append(s, other[i]);
}

str_t *str_alloc(const char *from) {
    str_t *s = (str_t *)mem_s_malloc(sizeof(str_t), NULL, NULL);
    s->data = (char *)mem_s_malloc(1, NULL, NULL);
    s->len = 0;
    s->cap = 1;
    s->data[0] = '\0';
    if (from)
        str_concat_from_cstr(s, from);
    return s;
}

str_t str_create(const char *from) {
    str_t s = (str_t) {
        .data = (char *)mem_s_malloc(1, NULL, NULL),
        .len = 0,
        .cap = 1,
    };

    s.data[0] = '\0';

    if (from)
        str_concat_from_cstr(&s, from);

    return s;
}

void str_clear(str_t *s) {
    s->data[0] = '\0';
    s->len = 0;
}

size_t str_len(const str_t *const s) {
    return s->len;
}
