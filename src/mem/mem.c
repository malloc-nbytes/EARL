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
#include <stdio.h>
#include <string.h>

#include "mem/mem.h"

void *mem_s_malloc(size_t bytes, arena_t *arena, void *(allocator)(arena_t *, size_t)) {
    void *p = (allocator != NULL && arena != NULL)
        ? allocator(arena, bytes)
        : malloc(bytes);
    if (!p) {
        fprintf(stderr, "could not safely allocate %zu bytes\n", bytes);
        exit(1);
    }
    return p;
}

void *mem_heapify(void *src, size_t bytes) {
    void *dest = mem_s_malloc(bytes, NULL, NULL);
    return memcpy(dest, src, bytes);
}
