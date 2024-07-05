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

#include <cassert>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

OS::OS() {}

bool OS::mkdir(Obj *path) {
    UNIMPLEMENTED("OS::mkdir");
}

bool OS::dir_exists(Obj *path) {
    UNIMPLEMENTED("OS::dir_exists");
}

bool OS::rmdir(Obj *path) {
    UNIMPLEMENTED("OS::rmdir");
}

bool OS::file_exists(Obj *path) {
    UNIMPLEMENTED("OS::file_exists");
}

std::vector<Obj *> OS::ls(Obj *path) {
    UNIMPLEMENTED("OS::ls");
}

/*** OVERRIDES ***/
Type OS::type(void) const {
    UNIMPLEMENTED("OS::type");
}

Obj *OS::binop(Token *op, Obj *other) {
    UNIMPLEMENTED("OS::binop");
}

bool OS::boolean(void) {
    UNIMPLEMENTED("OS::boolean");
}

void OS::mutate(Obj *other) {
    UNIMPLEMENTED("OS::mutate");
}

Obj *OS::copy(void) {
    UNIMPLEMENTED("OS::copy");
}
bool OS::eq(Obj *other) {
    UNIMPLEMENTED("OS::eq");
}