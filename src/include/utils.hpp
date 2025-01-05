/** @file */

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

#include <string>

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

// Macro to print `msg` and fail.
#define UNIMPLEMENTED(msg)                              \
    do {                                                \
        fprintf(stderr, "UNIMPLEMENTED: " msg ":" __FILE__ "\n");        \
        exit(1);                                                        \
    } while (0)                                         \

// Used to silence compiler warning on a function with
// some return value that is not yet returning anything.
// It prints `msg` and returns `retttype`
#define UNIMPLEMENTED_WITH(msg, rettype)                \
    fprintf(stderr, "[EARL UNIMPLEMENTED]: " msg "\n");        \
    return rettype

#define TODO(msg) fprintf(stderr, "[EARL TODO]: " msg "\n")

int levenshtein_distance(std::string &s, std::string &t);
std::string file_to_cxxstring(const std::string &path);
bool is_number(const std::string &n);

#endif // UTILS_H
