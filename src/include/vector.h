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

// File: vector.h
// Description:
//   A vector interface. It is a generic array
//   that grows by a factor of x2 when the length
//   matches the capacity.

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdint.h>

#include "utils.h"

struct vector {
  uint8_t *data; // The underlying "generic" data.
  size_t len;
  size_t cap;
  size_t stride; // The size of each element in `data`.
};

// Macro to make it clear what kind of
// element the vector is going to hold.
// Example:
//   struct vector v = vector_create2(int);
#define vector_create2(type)                 \
  (struct vector) {                          \
    .data = utils_safe_malloc(sizeof(type)), \
    .cap = 1,                                \
    .len = 0,                                \
    .stride = sizeof(type),                  \
  };

// Gives you the element in the vector `v` at `idx` and casts
// it as `cast`.
#define vector_deref_at(v, idx, cast) (*((cast *)((v).data + (idx) * (v).stride)))

// Debug prints the vector `v` with the format string
// `formatstr` and casted as `type`.
#define vector_debug_dump(v, formatstr, type)           \
  do {                                                  \
    for (size_t i = 0; i < v.len; ++i) {                \
      printf(formatstr, vector_deref_at(v, i, type));  \
    }                                                   \
  } while (0)

// ONLY FOR EXPLICITNESS.
// Only used when instantiating
// a new vector. This allows to
// explicitly state what it holds,
// however nothing happens if the
// type does not match what it is
// actually holding.
// Example:
//   struct vector(int) v = vector_create(int);
//   struct F { struct vector(int) v, };
#define vector(type) vector

// Create a vector where the size of each
// element to be stored is `stride`.
struct vector vector_create(size_t stride);

// Appends `data` to the END of the vector.
void vector_append(struct vector *v, void *data);

// Retrieve an element at `i`. It is up to
// the caller to cast as necessary.
void *vector_at(struct vector *v, size_t i);

// Free the underlying data in `v`. Resets
// all member variables.
void vector_free(struct vector *v);

// Remove an element at a specific index.
void vector_rm_at(struct vector *v, size_t idx);

// Gives the underlying data as uint8_t.
uint8_t *vector_asbytes(struct vector *v);

int vector_empty(struct vector *v);

int vector_contains(struct vector *v, void *value);

void vector_pop(struct vector *v);

#endif // VECTOR_H
