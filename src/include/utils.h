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

// File: utils.h
// Description:
//   A bunch of helpful utility tools.

#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

// Debug assertions that are taken out
// when compiling in "release" mode.
// In "debug" mode, they will function.
#ifdef DEBUG
#define DEBUG_ASSERT(expr) assert(expr)
#else
#define DEBUG_ASSERT(expr) ((void)0)
#endif

// A macro that void's `x`. This is useful to silence compiler warnings.
#define NOOP(x) ((void)(x))

// Used to silence compiler warning on a function with
// some return value that is not yet returning anything.
// It prints `msg` and returns `retttype`
#define UNIMPLEMENTED(msg, rettype)            \
  fprintf(stderr, "UNIMPLEMENTED: " msg "\n"); \
  return rettype

#define CPL(x, ty) ((void *)&(ty){(x)})

// A malloc wrapper that checks if
// malloc failed. Allocates `bytes` number of bytes.
void *utils_safe_malloc(size_t bytes);

// A wrapper for strcmp that checks if the
// strings are equal. Returns 1 on success,
// and 0 on failure.
int utils_streq(const char *s1, const char *s2);

// Fill an array from 0..len with values 0..len.
void utils_iota_array(int *arr, size_t len);

#endif // UTILS_H
