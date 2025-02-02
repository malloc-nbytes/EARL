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

/// @brief The different opcodes
typedef enum {
    OPCODE_HALT        = 0x00,
    OPCODE_CONST       = 0x01,
    OPCODE_ADD         = 0x02,
    OPCODE_MINUS       = 0x03,
    OPCODE_MUL         = 0x04,
    OPCODE_DIV         = 0x05,
    OPCODE_MOD         = 0x06,
    OPCODE_STORE       = 0x07,
    OPCODE_LOAD        = 0x08,
    OPCODE_CALL        = 0x09,
    OPCODE_DEF_GLOBAL  = 0x0A,
    OPCODE_LOAD_GLOBAL = 0x0B,
    OPCODE_SET_GLOBAL  = 0x0C,
} opcode_t;

void opcode_dump(opcode_t op);

#endif // OPCODE_H
