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

#define OPCODE_HALT          0x00
#define OPCODE_CONST         0x01
#define OPCODE_ADD           0x02
#define OPCODE_MINUS         0x03
#define OPCODE_MUL           0x04
#define OPCODE_DIV           0x05
#define OPCODE_MOD           0x06
#define OPCODE_CALL          0x07
#define OPCODE_DEF_GLOBAL    0x08
#define OPCODE_LOAD_GLOBAL   0x09
#define OPCODE_SET_GLOBAL    0x0A
#define OPCODE_DEF_LOCAL     0x0B
#define OPCODE_LOAD_LOCAL    0x0C
#define OPCODE_SET_LOCAL     0x0D
#define OPCODE_POP           0x0E
#define OPCODE_JUMP_IF_FALSE 0x0F

#endif // OPCODE_H
