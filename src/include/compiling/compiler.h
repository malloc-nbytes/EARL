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

#include "runtime/VM/opcode.h"
#include "runtime/EARL-value.h"
#include "parsing/ast.h"
#include "compiling/ctx.h"

/// @brief Holds all compilation information
typedef struct cc {
    /// @brief Holds the entire context during compilation
    ctx_t *ctx;

    struct {
        opcode_t *data;
        size_t len;
        size_t cap;
    } opcode;

    struct {
        EARL_value_t *data;
        size_t len;
        size_t cap;
    } constants;

    struct {
        const char **data;
        size_t len;
        size_t cap;
    } gl_syms;
} cc_t;

/// @brief Compile a program into bytecode based off of an AST
/// @param prog The AST to compile
/// @return The result of the compiled program
cc_t cc_compile(program_t *prog);

size_t cc_write_to_const_pool(cc_t *cc, EARL_value_t value);

size_t cc_write_global(cc_t *cc, const char *id);

#endif // COMPILER_H
