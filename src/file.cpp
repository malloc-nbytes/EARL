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

#include <fstream>
#include <cassert>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

File::File(Str *fp, Str *mode, std::fstream) : m_fp(fp), m_mode(mode), m_open(false) {}

void File::set_open(void) {
    m_open = true;
}

void File::set_closed(void) {
    m_open = false;
}

void File::dump(void) const {
    UNIMPLEMENTED("File::dump");
}

void File::close(void) {
    UNIMPLEMENTED("File::close");
}

/*** OVERRIDES ***/
Type File::type(void) const {
    return Type::File;
}

Obj *File::binop(Token *op, Obj *other) {
    UNIMPLEMENTED("File::binop");
}

bool File::boolean(void) {
    UNIMPLEMENTED("File::boolean");
}

void File::mutate(Obj *other) {
    UNIMPLEMENTED("File::mutate");
}

Obj *File::copy(void) {
    UNIMPLEMENTED("File::copy");
}
