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

#ifndef EVM_H
#define EVM_H

#include <stddef.h>

#include "runtime/EARL-value.h"
#include "compiling/compiler.h"
#include "ds/s-umap.h"

#define STACK_LIM 512

/// @brief The EARL virtual machine
typedef struct EARL_vm {
    /// @brief The program stack
    struct {
        EARL_value_t *data[STACK_LIM];
        size_t len;
    } stack;

    /// @brief A map of all global variables
    s_umap_t(identifier_t *) globals;

    /// @brief The compilation context
    cc_t *cc;

    /// @brief The stack pointer
    EARL_value_t **sp;

    /// @brief The instruction pointer
    opcode_t *ip;

    /*** Routines ***/
    /// @brief Read the byte that is pointed to
    ///        by the instruction pointer.
    opcode_t (*read_byte)(struct EARL_vm *vm);

    /// @brief Initialize the VM
    void (*init)(struct EARL_vm *vm);

    /// @brief Push a value onto the stack
    void (*push)(struct EARL_vm *vm, EARL_value_t *value);

    /// @brief Pop a value off of the stack
    EARL_value_t *(*pop)(struct EARL_vm *vm);

    /// @brief Dump the stack
    void (*dump_stack)(struct EARL_vm *vm);
} EARL_vm_t;

/// @brief Execute the program
/// @param opcode The compiled bytecode
/// @param opcode_len The length of opcdoe
/// @param opcode_cap The capacity of opcdoe
/// @return The value after execution
EARL_value_t *EVM_exec(cc_t *cc);

#endif // EVM_H
