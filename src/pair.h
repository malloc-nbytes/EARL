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

#ifndef PAIR_H
#define PAIR_H

#include <stdint.h>
#include <stddef.h>

struct pair {
  uint8_t *fst;
  size_t fst_stride;
  uint8_t *snd;
  size_t snd_stride;
};

#define pair_create(t1, t2)                     \
  (struct pair) {                               \
    .fst = malloc(sizeof(t1)),                  \
    .fst_stride = sizeof(t1),                   \
    .snd = malloc(sizeof(t2)),                  \
    .snd_stride = sizeof(t2),                   \
  }

#define pair(t1, t2) pair

#define pair_unsafe_fst(p, cast) (*((cast*)(p).fst))
#define pair_unsafe_snd(p, cast) (*((cast*)(p).snd))

void pair_make_unique(struct pair *pair, void *fst, void *snd);
void *pair_fst(struct pair *p);
void *pair_snd(struct pair *p);
void pair_free(struct pair *p);

#endif // PAIR_H
