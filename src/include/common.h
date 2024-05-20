// MIT License

// Copyright (c) 2023 malloc-nbytes

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

// File: common.h
// Description:
//   A few things that are useful
//   throughout the entire project.

#ifndef COMMON_H
#define COMMON_H

// Keywords
#define COMMON_KW_LET    "let"
#define COMMON_KW_DEF    "def"
#define COMMON_KW_RETURN "return"

// Types
#define COMMON_TY_INT32 "int"
#define COMMON_TY_STR   "str"

// Make sure to add to these when adding
// new keywords and types.
#define COMMON_KW_AS_CPL {COMMON_KW_LET, COMMON_KW_DEF, COMMON_KW_RETURN}
#define COMMON_TY_AS_CPL {COMMON_TY_INT32, COMMON_TY_STR}

#endif // COMMON_H
