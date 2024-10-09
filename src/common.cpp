/** @file */

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

#include <vector>
#include <string>

#include "err.hpp"
#include "common.hpp"

void handle_flag(unsigned flag, bool set) {
    if (set)
        flags |= flag;
    else
        flags &= ~flag;
}

void handle_2flag(std::string flag, bool set) {
    if (flag == COMMON_EARL2ARG_WITHOUT_STDLIB)
        handle_flag(__WITHOUT_STDLIB, set);
    else if (flag == COMMON_EARL2ARG_REPL_NOCOLOR)
        handle_flag(__REPL_NOCOLOR, set);
    else if (flag == COMMON_EARL2ARG_WATCH)
        handle_flag(__WATCH, set);
    else if (flag == COMMON_EARL2ARG_SHOWFUNS)
        handle_flag(__SHOWFUNS, set);
    else if (flag == COMMON_EARL2ARG_CHECK)
        handle_flag(__CHECK, set);
    else if (flag == COMMON_EARL2ARG_TOPY)
        throw InterpreterException("cannot set flag `--" COMMON_EARL2ARG_TOPY "` during runtime");
    else if (flag == COMMON_EARL2ARG_VERBOSE)
        handle_flag(__VERBOSE, set);
    else if (flag == COMMON_EARL2ARG_SHOWBASH)
        handle_flag(__SHOWBASH, set);
    else if (flag == COMMON_EARL2ARG_SHOWLETS)
        handle_flag(__SHOWLETS, set);
    else if (flag == COMMON_EARL2ARG_SHOWMUTS)
        handle_flag(__SHOWMUTS, set);
    else if (flag == COMMON_EARL2ARG_NO_SANITIZE_PIPES)
        handle_flag(__NO_SANITIZE_PIPES, set);
    else
        throw InterpreterException("invalid flag `"+flag+"`");
}

void handle_1flag(char flag, bool set) {
    if (flag == COMMON_EARL1ARG_CHECK)
        handle_flag(__CHECK, set);
    else if (flag == COMMON_EARL1ARG_WATCH)
        handle_flag(__WATCH, set);
    else
        throw InterpreterException("invalid flag `"+std::string(1, flag)+"`");
}
