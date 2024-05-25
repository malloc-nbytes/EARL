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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "notify.h"
#include "utils.h"

void *
utils_safe_malloc(size_t bytes)
{
  void *p = malloc(bytes);
  if (!p) {
    assert(false && "fixme");
    // NOTIFY_ERRARGS(ERR_FATAL, "utils_safe_malloc failed when allocating %zu bytes", bytes);
  }
  return p;
}

int
utils_streq(const char *s1, const char *s2)
{
  return strcmp(s1, s2) == 0;
}

void
utils_iota_array(int *arr, size_t len)
{
  for (size_t i = 0; i < len; ++i) {
    arr[i] = i;
  }
}
