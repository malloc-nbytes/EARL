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
#include <string.h>

#include "ds/s-uset.h"
#include "misc/utils.h"

s_uset_t s_uset_create(unsigned long (*hash)(const char *)) {
    const size_t cap = 32;
    return (s_uset_t) {
        .tbl = (s_uset_node_t **)s_malloc(sizeof(s_uset_node_t *) * cap, NULL, NULL),
        .sz = 0,
        .cap = cap,
        .hash = hash,
    };
}

static void free_node(s_uset_node_t *it) {
    /* while (it) { */
    /*         if (it->value) */
    /*                 free(it->value); */
    /*         s_uset_node *tmp = it; */
    /*         it = it->next; */
    /*         free(tmp); */
    /* } */
}

void s_uset_destroy(s_uset_t *set) {
    for (size_t i = 0; i < set->cap; ++i)
        free_node(set->tbl[i]);
    set->sz = set->cap = 0;
}

s_uset_node_t **find_free_set_node_spot(s_uset_t *set, unsigned long idx, s_uset_node_t **prev) {
    s_uset_node_t **it = &set->tbl[idx];
    *prev = NULL;
    while (*it) {
        *prev = *it;
        it = &(*it)->next;
    }
    return it;
}

void s_uset_insert(s_uset_t *set, const char *value) {
    const size_t idx = set->hash(value) % set->cap;
    s_uset_node_t *prev = NULL;
    s_uset_node_t **node = find_free_set_node_spot(set, idx, &prev);
    *node = (s_uset_node_t *)s_malloc(sizeof(s_uset_node_t), NULL, NULL);
    (*node)->value = strdup(value);
    (*node)->next = NULL;
    if (prev)
        prev->next = *node;
    set->sz++;
}

void s_uset_remove(s_uset_t *set, const char *value) {
    (void)set;
    (void)value;
    TODO;
}

int s_uset_contains(s_uset_t *set, const char *value) {
    const size_t idx = set->hash(value) % set->cap;
    s_uset_node_t *it = set->tbl[idx];
    while (it && !streq(it->value, value))
        it = it->next;
    return it ? 1 : 0;
}
