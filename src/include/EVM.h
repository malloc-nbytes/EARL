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

#include "EARL-value.h"
#include "compiler.h"
#include "s-umap.h"

#define READ_BYTE(ip) *ip++
#define GET_CONST(cc, ip) cc->cp[READ_BYTE(ip)]
#define STACK_LIM 512

struct EARL_vm {
        struct {
                struct EARL_value *data[STACK_LIM];
                size_t len;
        } stack;

        struct s_umap globals;
        struct cc *cc;

        struct EARL_value **sp;
        enum opcode *ip;

        // Routines
        enum   opcode      (*read_byte)(struct EARL_vm *vm);
               void        (*init)(struct EARL_vm *vm);
               void        (*push)(struct EARL_vm *vm, struct EARL_value *value);
        struct EARL_value *(*pop)(struct EARL_vm *vm);
};

/// @brief Execute the program
/// @param opcode The compiled bytecode
/// @param opcode_len The length of opcdoe
/// @param opcode_cap The capacity of opcdoe
/// @return The value after execution
struct EARL_value *EVM_exec(struct cc *cc);

#endif // EVM_H
