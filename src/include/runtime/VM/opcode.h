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

#ifndef OPCODE_H
#define OPCODE_H

#include <stdint.h>

/**
 * @macro OPCODE_HALT
 * @brief Will immediately exit the program if encountered.
 */
#define OPCODE_HALT          0x00

/**
 * @macro OPCODE_CONST
 * @brief Pushes a constant onto the stack. Expects an index
 *        to be the very next instruction into the constants table.
 */
#define OPCODE_CONST         0x01

/**
 * @macro OPCODE_ADD
 * @brief Pops two values off of the stack, adds them,
 *        and pushes the result onto the stack.
 */
#define OPCODE_ADD           0x02

/**
 * @macro OPCODE_SUB
 * @brief Pops two values off of the stack, subtracts them,
 *        and pushes the result onto the stack.
 */
#define OPCODE_SUB           0x03

/**
 * @macro OPCODE_MUL
 * @brief Pops two values off of the stack, multiplies them,
 *        and pushes the result onto the stack.
 */
#define OPCODE_MUL           0x04

/**
 * @macro OPCODE_DIV
 * @brief Pops two values off of the stack, divides them,
 *        and pushes the result onto the stack.
 */
#define OPCODE_DIV           0x05

/**
 * @macro OPCODE_MOD
 * @brief Pops two values off of the stack, mods them,
 *        and pushes the result onto the stack.
 */
#define OPCODE_MOD           0x06

/**
 * @macro OPCODE_CALL
 * @brief Does a function call with either a builtin function
 *        reference, or a user defined function. Expects the next
 *        byte to be the number of arguments given to it, and pops
 *        the function reference off of the stack. For every argument,
 *        it will pop it off of the stack. Finally, after the function
 *        call is evaluated, the result of the function will be pushed
 *        back onto the stack.
 */
#define OPCODE_CALL          0x07

/**
 * @macro OPCODE_DEF_GLOBAL
 * @brief Defines a global variable. The next byte must be and index
 *        into the constants table to grab the identifier. The top value
 *        of the stack will be popped off and used as the variable's value.
 */
#define OPCODE_DEF_GLOBAL    0x08

/**
 * @macro OPCODE_DEF_GLOBAL
 * @brief Loads a global variable and puts it's value onto the stack.
 *        It expects the next byte to be an index into the constants table
 *        to grab the identifier.
 */
#define OPCODE_LOAD_GLOBAL   0x09

/**
 * @macro OPCODE_SET_GLOBAL
 * @brief Sets the value of a global variable. Expects the next byte to
 *        be an index into the constants pool to grab the identifier.
 *        The top value on the stack will be popped off and used as
 *        the value of the global variable.
 */
#define OPCODE_SET_GLOBAL    0x0A

/**
 * @macro OPCODE_DEF_LOCAL
 * @brief Defines a local variable. It expects the next byte to be
 *        an index into the stack for storage. The top value of the stack
 *        will be used as its value.
 */
#define OPCODE_DEF_LOCAL     0x0B

/**
 * @macro OPCODE_LOAD_LOCAL
 * @brief Load a local variable. Expects the next byte to be an index
 *        into the stack where the variable is stored. Pushes the value
 *        of the variable onto the stack.
 */
#define OPCODE_LOAD_LOCAL    0x0C

/**
 * @macro OPCODE_SET_LOCAL
 * @brief Sets a local variable. Expects the next byte to be the index
 *        into the stack where the variable is stored. The value on the
 *        top of the stack will be popped off and used as the value of
 *        the variable.
 */
#define OPCODE_SET_LOCAL     0x0D

/**
 * @macro OPCODE_POP
 * @brief Pops a value off of the top of the stack.
 */
#define OPCODE_POP           0x0E

/**
 * @macro OPCODE_JUMP_IF_FALSE
 * @brief Will jump to a byte if the condition is false. It expects
 *        the next byte to be an offset to the byte to jump to if false.
 *        The value on the top of the stack will be popped off and used
 *        to check if it evaluates to false. If it is false, the instruction
 *        pointer will jump to the offest.
 */
#define OPCODE_JUMP_IF_FALSE 0x0F

/**
 * @macro OPCODE_JUMP
 * @brief Will jump to a specified byte. It expects the next byte to
 *        be an offset to the byte to jump to. It will then increment
 *        the instruction pointer to point to that byte.
 */
#define OPCODE_JUMP          0x10

/**
 * @macro OPCODE_JUMP_IF_TRUE
 * @brief Will jump to a byte if the condition is true. It expects
 *        the next byte to be an offset to the byte to jump to if true.
 *        The value on the top of the stack will be popped off and used
 *        to check if it evaluates to true. If it is true, the instruction
 *        pointer will jump to the offest.
 */
#define OPCODE_JUMP_IF_TRUE  0x11

/**
 * @macro OPCODE_LOOP
 * @brief (broken) Will go back a specified number of bytes. It expects
 *        the next byte to be an offset to the byte to jump to. The instruction
 *        pointer will then be subtracted to point to the specified byte.
 */
#define OPCODE_LOOP          0x12

/**
 * @macro OPCODE_LT
 * @brief Check if n1 == n2. Pops two values off of the stack (n2, n1)
 *        and checks n1 == n2. The result will be pushed back
 *        onto the stack.
 */
#define OPCODE_EQ            0x13

/**
 * @macro OPCODE_LT
 * @brief Check if n1 != n2. Pops two values off of the stack (n2, n1)
 *        and checks n1 != n2. The result will be pushed back
 *        onto the stack.
 */
#define OPCODE_NEQ           0x14

/**
 * @macro OPCODE_LT
 * @brief Check if n1 < n2. Pops two values off of the stack (n2, n1)
 *        and checks n1 < n2. The result will be pushed back
 *        onto the stack.
 */
#define OPCODE_LT            0x15

/**
 * @macro OPCODE_GT
 * @brief Check if n1 > n2. Pops two values off of the stack (n2, n1)
 *        and checks n1 > n2. The result will be pushed back
 *        onto the stack.
 */
#define OPCODE_GT            0x16

/**
 * @macro OPCODE_LE
 * @brief Check if n1 <= n2. Pops two values off of the stack (n2, n1)
 *        and checks n1 <= n2. The result will be pushed back
 *        onto the stack.
 */
#define OPCODE_LE            0x17

/**
 * @macro OPCODE_LE
 * @brief Check if n1 >= n2. Pops two values off of the stack (n2, n1)
 *        and checks n1 >= n2. The result will be pushed back
 *        onto the stack.
 */
#define OPCODE_GE            0x18

typedef struct {
    uint8_t *data;
    size_t len;
    size_t cap;
} opcode_t;

#endif // OPCODE_H
