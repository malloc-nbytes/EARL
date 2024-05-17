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

// File: hashtable.h
// Description:
//   Provides an API for a generic hashtable
//   utilizing tree-like adjacency lists.

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include <stddef.h>

struct hashtbl;

#define hashtbl_create2(key_ty, value_ty, hashf, keyc)          \
  (struct hashtbl) {                                            \
    .tbl = NULL,                                                \
    .hashfunc = hashf,                                          \
    .keycompar = keyc,                                          \
    .key_stride = sizeof(key_ty),                               \
    .value_stride = sizeof(value_ty),                           \
    .len = 0,                                                   \
    .cap = 0,                                                   \
  }

#define hashtbl(ty1, ty2) hashtbl

struct hashtbl hashtbl_create(size_t key_stride, size_t value_stride,
                              unsigned (*hashfunc)(void *key, size_t bytes),
                              int (*keycompar)(void *x, void *y));

void hashtbl_insert(struct hashtbl *ht, void *key, void *value, size_t bytes);

#endif // HASHTABLE_H
