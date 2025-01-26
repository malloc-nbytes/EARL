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

#include "opcode.h"
#include "ast.h"
#include "earl-value.h"

/// @brief Holds all compilation information
struct cc {
        /// @brief Holds the entire context during compilation
        struct ctx *ctx;

        struct {
                /// @brief The opcode that is produced
                enum opcode *data;

                /// @brief The length of the opcode
                size_t len;

                /// @brief The capacity of the opcode
                size_t cap;
        } opcode;

        struct {
                /// @brief The constant pool
                struct earl_value **data;

                /// @brief The length of the constant pool
                size_t len;

                /// @brief The capacity of the constant pool
                size_t cap;
        } const_pool;

        struct {
                /// @brief All symbols found
                const char **data;

                /// @brief The length of the symbols
                size_t len;

                /// @brief The capacity of the symbols
                size_t cap;
        } gl_syms;
};

/// @brief Compile a program into bytecode based off of an AST
/// @param prog The AST to compile
/// @return The result of the compiled program
struct cc cc_compile(struct program *prog);

size_t cc_push_constant(struct cc *cc, enum earl_value_type type, void *data);

void cc_dump_opcode(const struct cc cc);

#endif // COMPILER_H
