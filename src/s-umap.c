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

s_umap_t s_umap_create(unsigned long (*hash)(const char *), void (*destroy_value)(uint8_t *)) {
    assert(hash);
    const size_t cap = 64;
    return (s_umap_t) {
        .tbl = s_malloc(sizeof(s_umap_bucket_t *) * cap, NULL, NULL),
        .sz = 0,
        .cap = cap,
        .hash = hash,
        .destroy_value = destroy_value,
    };
}

s_umap_bucket_t **find_free_spot(s_umap_t *map, unsigned long idx, s_umap_bucket_t **prev) {
    s_umap_bucket_t **it = &map->tbl[idx];
    *prev = NULL;
    while (*it) {
        *prev = *it;
        it = &(*it)->next;
    }
    return it;
}

void s_umap_remove(s_umap_t *map, const char *key) {
    (void)map;
    (void)key;
    TODO;
}

void s_umap_insert(s_umap_t *map, const char *key, uint8_t *value) {
    const unsigned long idx = map->hash(key) % map->cap;
    s_umap_bucket_t *prev = NULL;
    s_umap_bucket_t **node = find_free_spot(map, idx, &prev);

    *node = (s_umap_bucket_t *)s_malloc(sizeof(s_umap_bucket_t), NULL, NULL);

    (*node)->key = strdup(key);
    (*node)->value = value;
    (*node)->next = NULL;

    if (prev)
        prev->next = *node;

    map->sz++;
}

void *s_umap_get(s_umap_t *map, const char *key) {
    const unsigned long idx = map->hash(key) % map->cap;
    s_umap_bucket_t *it = map->tbl[idx];
    while (it && !streq(it->key, key))
        it = it->next;
    return it ? it->value : NULL;
}

int s_umap_contains(s_umap_t *map, const char *key) {
    return s_umap_get(map, key) != NULL;
}

void s_umap_dump(s_umap_t *map, void (*display)(const uint8_t *)) {
    assert(display);
    for (size_t i = 0; i < map->cap; ++i) {
        s_umap_bucket_t *it = map->tbl[i];
        while (it) {
            printf("key: %s, value: ", it->key);
            display(map->tbl[i]->value);
            putchar('\n');
            it = it->next;
        }
    }
}

void s_umap_destroy(s_umap_t *map) {
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
