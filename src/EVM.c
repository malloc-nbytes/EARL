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
#include "opcode.h"
#include "utils.h"
#include "EARL-value.h"
#include "err.h"
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

        struct EARL_value **sp;
        enum opcode *ip;

        enum opcode (*read_byte)(struct EARL_vm *);
};

enum opcode
read_byte(struct EARL_vm *vm) {
        return *vm->ip++;
}

void
stack_push(struct EARL_vm *vm, struct EARL_value *value) {
    if (vm->stack.len >= STACK_LIM)
        err("Stack Overflow");

    *vm->sp = value;
    *vm->sp++;
    vm->stack.len++;
}

struct EARL_value *
stack_pop(struct EARL_vm *vm) {
    if (vm->stack.len == 0)
        err("Stack Underflow");

    *vm->sp--;
    vm->stack.len--;
    return *vm->sp;
}

struct EARL_value *
EVM_exec(struct cc *cc) {
        printf("Begin Interpreter...\n");

        struct EARL_vm vm = (struct EARL_vm) {
                .stack = { .len = 0 },
                .globals = NULL,
                .sp = NULL,
                .ip = &cc->opcode.data[0],
                .read_byte = read_byte,
        };

        vm.sp = &vm.stack.data[0];

        // Begin interpretation
        while (1) {
                int b = 0;

                enum opcode instr;

                switch (instr = vm.read_byte(&vm)) {
                case OPCODE_HALT: b = 1; break;
                case OPCODE_CONST: {
                        size_t idx = vm.read_byte(&vm);
                        struct EARL_value *constant = cc->const_pool.data[idx];
                        stack_push(&vm, constant);
                } break;
                case OPCODE_MINUS:
                case OPCODE_MUL:
                case OPCODE_DIV:
                case OPCODE_ADD: {
                        struct EARL_value *n1 = stack_pop(&vm);
                        struct EARL_value *n2 = stack_pop(&vm);

                        int v1 = EARL_value_get_int(n1),
                                v2 = EARL_value_get_int(n2);

                        int res = instr == OPCODE_ADD ? v2 + v1
                                : instr == OPCODE_MINUS ? v2 - v1
                                : instr == OPCODE_MUL ? v2 * v1
                                : instr == OPCODE_DIV ? v2 / v1
                                : v2 % v1;

                        struct EARL_value *res_value = EARL_value_alloc(EARL_VALUE_TYPE_INTEGER, &res);
                        stack_push(&vm, res_value);
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

        return stack_pop(&vm);
}
