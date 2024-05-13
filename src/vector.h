// MIT License

// Copyright (c) 2023 malloc-nbytes

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

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdint.h>

struct vector {
  uint8_t *data;
  size_t len;
  size_t cap;
  size_t stride;
};

#define vector_create(type)       \
  (struct vector) {               \
    .data = malloc(sizeof(type)), \
    .cap = 1,                     \
    .len = 0,                     \
    .stride = sizeof(type),       \
  };

// Does not do anything.
// Only used when instantiating
// a new vector. This allows to
// explicitly state what it holds,
// however nothing happens if the
// type does not match what it is
// actually holding.
#define vector(...) vector

void vector_append(struct vector *v, void *data);
void *vector_at(struct vector *v, size_t i);
void vector_free(struct vector *v);

#endif // VECTOR_H
