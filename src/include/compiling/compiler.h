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

#ifndef COMPILER_H
#define COMPILER_H

#include <stddef.h>
#include <stdint.h>

#define LOCALS_LIM UINT8_MAX + 1

#include "runtime/VM/opcode.h"
#include "runtime/EARL-value.h"
#include "parsing/ast.h"

typedef struct {
    const char *id;
    int depth;
} local_t;

/// @brief Holds all compilation information
typedef struct cc {
    struct {
        uint8_t *data;
        size_t len;
        size_t cap;
    } opcode;

    struct {
        EARL_value_t *data;
        size_t len;
        size_t cap;
    } constants;

    struct {
        local_t data[LOCALS_LIM];
        size_t len;
    } locals;

    size_t scope_depth;
} cc_t;

/// @brief Compile a program into bytecode based off of an AST
/// @param prog The AST to compile
/// @return The result of the compiled program
cc_t cc_compile(program_t *prog);

uint8_t cc_write_to_const_pool(cc_t *cc, EARL_value_t value);

uint8_t cc_write_str_to_const(cc_t *cc, const char *id);

#endif // COMPILER_H
