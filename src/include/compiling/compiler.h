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
#include "parsing/ast.h"
#include "runtime/EARL-value.h"
#include "compiling/ctx.h"

/// @brief Holds all compilation information
typedef struct cc {
    /// @brief Holds the entire context during compilation
    ctx_t *ctx;

    struct {
        /// @brief The opcode that is produced
        opcode_t *data;

        /// @brief The length of the opcode
        size_t len;

        /// @brief The capacity of the opcode
        size_t cap;
    } opcode;

    struct {
        /// @brief The constant pool
        EARL_value_t **data;

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
} cc_t;

/// @brief Compile a program into bytecode based off of an AST
/// @param prog The AST to compile
/// @return The result of the compiled program
cc_t cc_compile(program_t *prog);

/// @brief Push a constant into the CC
/// @param cc The compiler context
/// @return The index that the constant was pushed
///         into the contant pool
size_t cc_push_constant(cc_t *cc, EARL_value_type_t type, void *data);

/// @brief Push an identifier into the global scope
/// @param cc The compiler context
/// @return The index that the global was pushed
///         into the global symbols table
size_t cc_push_global(cc_t *cc, const char *id);

/// @brief Dump all opcode
/// @param cc The compiler context
void cc_dump_opcode(const cc_t cc);

/// @brief Dump all global symbols
/// @param cc The compiler context
void cc_dump_gl_syms(const cc_t *cc);

#endif // COMPILER_H
