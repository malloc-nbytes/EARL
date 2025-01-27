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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "EVM.h"
#include "EVM-routines.h"
#include "opcode.h"
#include "utils.h"
#include "EARL-value.h"
#include "err.h"
#include "hash.h"
#include "s-umap.h"

struct EARL_value *
EVM_exec(struct cc *cc) {
        printf("Begin Interpreter...\n");

        struct EARL_vm vm = (struct EARL_vm) {
                .stack     = { .len = 0 },
                .globals   = s_umap_create(djb2, NULL),
                .cc        = cc,
                .sp        = NULL,
                .ip        = NULL,

                .read_byte = EVM_routines_read_byte,
                .init      = EVM_routines_init,
                .push      = EVM_routines_stack_push,
                .pop       = EVM_routines_stack_pop,
        };

        vm.init(&vm);

        // Begin interpretation
        while (1) {
                int b = 0;

                enum opcode instr;

                switch (instr = vm.read_byte(&vm)) {
                case OPCODE_HALT: b = 1; break;
                case OPCODE_CONST: {
                        size_t idx = vm.read_byte(&vm);
                        struct EARL_value *constant = cc->const_pool.data[idx];
                        vm.push(&vm, constant);
                } break;
                case OPCODE_MINUS:
                case OPCODE_MUL:
                case OPCODE_DIV:
                case OPCODE_ADD: {
                        struct EARL_value *n1 = vm.pop(&vm);
                        struct EARL_value *n2 = vm.pop(&vm);

                        int v1 = EARL_value_get_int(n1),
                                v2 = EARL_value_get_int(n2);

                        int res = instr == OPCODE_ADD ? v2 + v1
                                : instr == OPCODE_MINUS ? v2 - v1
                                : instr == OPCODE_MUL ? v2 * v1
                                : instr == OPCODE_DIV ? v2 / v1
                                : v2 % v1;

                        struct EARL_value *res_value = EARL_value_alloc(EARL_VALUE_TYPE_INTEGER, &res);
                        vm.push(&vm, res_value);
                } break;
                case OPCODE_STORE: {
                        assert(0);
                        // size_t idx = vm.read_byte(&vm);
                        // struct EARL_value *value = stack_pop(&vm);
                        // cc->const_pool.data[idx] = value;
                } break;
                default: {
                        fprintf(stderr, "unknown instruction %#x\n", instr);
                        exit(1);
                }
                }

                if (b) break;
        }

        return vm.pop(&vm);
}
