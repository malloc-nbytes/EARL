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

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/arena.h"

#define AUTO(id, v) typeof(v) id = v

#define TODO                                                    \
    fprintf(stderr, "!!! TODO: %s() !!!\n", __FUNCTION__);      \
    exit(1);

#define da_append(arr, len, cap, ty, value)                       \
    do {                                                          \
        if ((cap) == 0)                                           \
            fprintf(stderr, "utils_da_append: `cap` is 0\n");     \
        if ((len) >= (cap)) {                                     \
            (cap) = !(cap) ? 2 : (cap) * 2;                       \
            (arr) = (ty)realloc((arr), (cap) * sizeof((arr)[0])); \
        }                                                         \
        (arr)[(len)] = (value);                                   \
        (len) += 1;                                               \
    } while (0)

/// @brief Read in a file to a cstr
/// @param fp The filepath
/// @return A const char * of the file's contents
char *read_file(const char *fp);

/// @brief Compare two strings to see if they are equal
/// @param s1 The first string
/// @param s2 The second string
/// @return 1 if they are equal, 0 if not
int streq(const char *s1, const char *s2);

#endif // UTILS_H
