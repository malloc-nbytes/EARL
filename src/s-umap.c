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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s-umap.h"
#include "utils.h"

struct s_umap
s_umap_create(unsigned long (*hash)(const char *), void (*destroy_value)(uint8_t *)) {
        assert(hash);
        const size_t cap = 64;
        return (struct s_umap) {
                .tbl = utils_s_malloc(sizeof(struct s_umap_bucket *) * cap, NULL, NULL),
                .sz = 0,
                .cap = cap,
                .hash = hash,
                .destroy_value = destroy_value,
        };
}

struct s_umap_bucket **
find_free_spot(struct s_umap *map, unsigned long idx, struct s_umap_bucket **prev) {
        struct s_umap_bucket **it = &map->tbl[idx];
        *prev = NULL;
        while (*it) {
                *prev = *it;
                it = &(*it)->next;
        }
        return it;
}

void
s_umap_remove(struct s_umap *map, const char *key) {
        (void)map;
        (void)key;
        TODO;
}

void
s_umap_insert(struct s_umap *map, const char *key, uint8_t *value) {
        const unsigned long idx = map->hash(key) % map->cap;
        struct s_umap_bucket *prev = NULL;
        struct s_umap_bucket **node = find_free_spot(map, idx, &prev);

        *node = (struct s_umap_bucket *)utils_s_malloc(sizeof(struct s_umap_bucket), NULL, NULL);

        (*node)->key = strdup(key);
        (*node)->value = value;
        (*node)->next = NULL;

        if (prev)
                prev->next = *node;

        map->sz++;
}

void *
s_umap_get(struct s_umap *map, const char *key) {
        const unsigned long idx = map->hash(key) % map->cap;
        struct s_umap_bucket *it = map->tbl[idx];
        while (it && !utils_streq(it->key, key))
                it = it->next;
        return it ? it->value : NULL;
}

int
s_umap_contains(struct s_umap *map, const char *key) {
        return s_umap_get(map, key) != NULL;
}

void
s_umap_dump(struct s_umap *map, void (*display)(const uint8_t *)) {
        assert(display);
        for (size_t i = 0; i < map->cap; ++i) {
                struct s_umap_bucket *it = map->tbl[i];
                while (it) {
                        printf("key: %s, value: ", it->key);
                        display(map->tbl[i]->value);
                        putchar('\n');
                        it = it->next;
                }
        }
}

void
s_umap_destroy(struct s_umap *map) {
        for (size_t i = 0; i < map->sz; ++i)
                if (map->tbl[i]) {
                        if (map->destroy_value)
                                map->destroy_value(map->tbl[i]->value);
                        free(map->tbl[i]->key);
                        free(map->tbl[i]);
                }
        free(map->tbl);
        map->tbl = NULL;
        map->sz = map->cap = 0;
}
