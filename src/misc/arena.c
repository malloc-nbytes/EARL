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

#include "misc/utils.h"
#include "misc/arena.h"

arena_t arena_create(size_t *bytes) {
    size_t N = bytes ? *bytes : ARENA_DEFAULT_SZ;
    return (arena_t) {
        .mem = (uint8_t *)s_malloc(sizeof(uint8_t) * N, NULL, NULL),
        .len = 0,
        .cap = N,
    };
}

void arena_create_from(arena_t *arena, size_t *bytes) {
    size_t N = bytes ? *bytes : ARENA_DEFAULT_SZ;
    arena->mem = (uint8_t *)s_malloc(sizeof(uint8_t) * N, NULL, NULL);
    arena->len = 0;
    arena->cap = N;
}

uint8_t *arena_malloc(arena_t *arena, size_t bytes) {
    if (!arena)
        return NULL;

    if (arena->len + bytes > arena->cap) {
        arena->cap *= 2;
        arena->mem = realloc(arena->mem, arena->cap);
    }

    if (!arena->mem)
        return NULL;

    uint8_t *mem = &arena->mem[arena->len];
    arena->len += bytes;
    return mem;
}

void arena_free(arena_t *arena) {
    free(arena->mem);
    arena->mem = NULL;
    arena->len = arena->cap = 0;
}
