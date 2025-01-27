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

#define READ_BYTE(ip) *ip++
#define GET_CONST(cc, ip) cc->cp[READ_BYTE(ip)]
#define STACK_LIM 512

/* struct EARL_vm { */
/*         struct { */
/*                 struct EARL_value *data[STACK_LIM]; */
/*                 size_t len; */
/*         } stack; */
/* }; */

struct stack {
        struct EARL_value *data[STACK_LIM];
        size_t len;
};

void
stack_push(struct stack *stack, struct EARL_value ***sp, struct EARL_value *value) {
    if (stack->len >= STACK_LIM) {
        err("Stack Overflow");
    }

    **sp = value;
    (*sp)++;
    stack->len++;
}

struct EARL_value *
stack_pop(struct stack *stack, struct EARL_value ***sp) {
    if (stack->len == 0) {
        err("Stack Underflow");
    }

    (*sp)--;
    stack->len--;
    return **sp;
}

struct EARL_value *
EVM_exec(struct cc *cc) {
        printf("Begin Interpreter...\n");

        // Epilogue
        struct stack stack = (struct stack) { .len = 0 };

        // Pointers
        struct EARL_value **sp = &stack.data[0];
        enum opcode *ip = &cc->opcode.data[0];

        // Begin interpretation
        while (1) {
                int b = 0;

                enum opcode instr;

                switch (instr = READ_BYTE(ip)) {
                case OPCODE_HALT: b = 1; break;
                case OPCODE_CONST: {
                        size_t idx = READ_BYTE(ip);
                        struct EARL_value *constant = cc->const_pool.data[idx];
                        stack_push(&stack, &sp, constant);
                } break;
                case OPCODE_MINUS:
                case OPCODE_MUL:
                case OPCODE_DIV:
                case OPCODE_ADD: {
                        struct EARL_value *n1 = stack_pop(&stack, &sp);
                        struct EARL_value *n2 = stack_pop(&stack, &sp);

                        int v1 = EARL_value_get_int(n1),
                                v2 = EARL_value_get_int(n2);

                        int res = instr == OPCODE_ADD ? v2 + v1
                                : instr == OPCODE_MINUS ? v2 - v1
                                : instr == OPCODE_MUL ? v2 * v1
                                : instr == OPCODE_DIV ? v2 / v1
                                : v2 % v1;

                        struct EARL_value *res_value = EARL_value_alloc(EARL_VALUE_TYPE_INTEGER, &res);
                        stack_push(&stack, &sp, res_value);
                } break;
                case OPCODE_STORE: {
                        size_t idx = READ_BYTE(ip);
                        struct EARL_value *value = stack_pop(&stack, &sp);
                        cc->const_pool.data[idx] = value;
                } break;
                default: {
                        fprintf(stderr, "unknown instruction %#x\n", instr);
                        exit(1);
                }
                }

                if (b) break;
        }

        return stack_pop(&stack, &sp);
}
