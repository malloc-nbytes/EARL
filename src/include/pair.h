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

// File: pair.h
// Description:
//   Provides an API for a `pair` container.
//   A pair contains two generic values.

#ifndef PAIR_H
#define PAIR_H

#include <stdint.h>
#include <stddef.h>
#include "utils.h"

struct pair {
  uint8_t *fst;
  size_t fst_stride;

  uint8_t *snd;
  size_t snd_stride;
};

// Create a pair from two types given.
#define pair_create2(t1, t2)                    \
  (struct pair) {                               \
    .fst = utils_safe_malloc(sizeof(t1)),       \
    .fst_stride = sizeof(t1),                   \
    .snd = utils_safe_malloc(sizeof(t2)),       \
    .snd_stride = sizeof(t2),                   \
  }

// Syntax sugar when creating a new pair.
// Allows to display the types that it holds.
// Nothing happens if this is not used, or if
// the values are not correct.
#define pair(t1, t2) pair

// Get the first value in the pair. It will attempt
// to cast to the type that is given.

#define pair_deref_fst(p, cast) (*((cast*)(p).fst))

// Get the second value in the pair. It will attempt
// to cast to the type that is given.
#define pair_deref_snd(p, cast) (*((cast*)(p).snd))

struct pair pair_create(size_t fst_stride, size_t snd_stride);

struct pair pair_from(void *fst, size_t fst_stride, void *snd, size_t snd_stride);

// Insert two values into the pair.
// NOTE: Only up to pair->fst_stride will be
// copied into pair->fst, and likewise, pair->snd
// will only copy pair->snd_stride bytes.
void pair_make_unique(struct pair *pair, void *fst, void *snd);

// Get the first value in the pair.
// NOTE: It is up to the caller to dereference
// it as needed.
void *pair_fst(struct pair *p);

// Get the second value in the pair.
// NOTE: It is up to the caller to dereference
// it as needed.
void *pair_snd(struct pair *p);

// Free the underlying memory in the pair.
void pair_free(struct pair *p);

#endif // PAIR_H
