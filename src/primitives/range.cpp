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
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Range::Range(int start, int end, int stride, Type range_ty)
    : m_start(start), m_end(end), m_stride(stride), m_range_ty(range_ty) {}

Type
Range::type(void) const {
    UNIMPLEMENTED("Range::type");
}

std::shared_ptr<Obj>
Range::binop(Token *op, std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Range::binop");
}

bool
Range::boolean(void) {
    UNIMPLEMENTED("Range::boolean");
}

void
Range::mutate(const std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Range::mutate");
}

std::shared_ptr<Obj>
Range::copy(void) {
    UNIMPLEMENTED("Range::mutate");
}

bool
Range::eq(std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Range::eq");
}

std::string
Range::to_cxxstring(void) {
    UNIMPLEMENTED("Range::to_cxxstring");
}

void
Range::spec_mutate(Token *op, const std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Range::spec_mutate");
}

std::shared_ptr<Obj>
Range::unaryop(Token *op) {
    UNIMPLEMENTED("Range::unaryop");
}
