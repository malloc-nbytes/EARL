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

#include <assert.h>
#include <string.h>

#include "utils.h"
#include "pair.h"

void
pair_make_unique(struct pair *pair, void *fst, void *snd)
{
  DEBUG_ASSERT(pair->fst && pair->snd);
  (void)memcpy(pair->fst, fst, pair->fst_stride);
  (void)memcpy(pair->snd, snd, pair->snd_stride);
}

void *
pair_fst(struct pair *p)
{
  DEBUG_ASSERT(pair->fst);
  return p->fst;
}

void *
pair_snd(struct pair *p)
{
  DEBUG_ASSERT(pair->snd);
  return p->snd;
}

void
pair_free(struct pair *p)
{
  free(p->fst);
  free(p->snd);
}
