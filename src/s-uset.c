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

#include "s-uset.h"
#include "utils.h"

struct s_uset
s_uset_create(unsigned long (*hash)(const char *)) {
        const size_t cap = 32;
        return (struct s_uset) {
                .tbl = (struct s_uset_node **)s_malloc(sizeof(struct s_uset_node *) * cap, NULL, NULL),
                .sz = 0,
                .cap = cap,
                .hash = hash,
        };
}

static void
free_node(struct s_uset_node *it) {
        /* while (it) { */
        /*         if (it->value) */
        /*                 free(it->value); */
        /*         struct s_uset_node *tmp = it; */
        /*         it = it->next; */
        /*         free(tmp); */
        /* } */
}

void
s_uset_destroy(struct s_uset *set) {
        for (size_t i = 0; i < set->cap; ++i)
                free_node(set->tbl[i]);
        set->sz = set->cap = 0;
}

struct s_uset_node **
find_free_set_node_spot(struct s_uset *set, unsigned long idx, struct s_uset_node **prev) {
        struct s_uset_node **it = &set->tbl[idx];
        *prev = NULL;
        while (*it) {
                *prev = *it;
                it = &(*it)->next;
        }
        return it;
}

void
s_uset_insert(struct s_uset *set, const char *value) {
        const size_t idx = set->hash(value) % set->cap;
        struct s_uset_node *prev = NULL;
        struct s_uset_node **node = find_free_set_node_spot(set, idx, &prev);
        *node = (struct s_uset_node *)s_malloc(sizeof(struct s_uset_node),
                                                     NULL, NULL);
        (*node)->value = strdup(value);
        (*node)->next = NULL;
        if (prev)
                prev->next = *node;
        set->sz++;
}

void
s_uset_remove(struct s_uset *set, const char *value) {
        (void)set;
        (void)value;
        TODO;
}

int
s_uset_contains(struct s_uset *set, const char *value) {
        const size_t idx = set->hash(value) % set->cap;
        struct s_uset_node *it = set->tbl[idx];
        while (it && !streq(it->value, value))
                it = it->next;
        return it ? 1 : 0;
}
