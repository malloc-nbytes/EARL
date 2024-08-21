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

/*** OVERRIDES ***/
Type Continue::type(void) const {
    return Type::Continue;
}

std::shared_ptr<Obj> Continue::binop(Token *op, std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Continue::binop");
}

bool Continue::boolean(void) {
    UNIMPLEMENTED("Continue::boolean");
}

void Continue::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    UNIMPLEMENTED("Continue::mutate");
}

std::shared_ptr<Obj> Continue::copy(void) {
    UNIMPLEMENTED("Continue::mutate");
}

bool Continue::eq(std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Continue::eq");
}

std::string Continue::to_cxxstring(void) {
    UNIMPLEMENTED("Continue::to_cxxstring");
}

void Continue::spec_mutate(Token *op, const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    UNIMPLEMENTED("Continue::spec_mutate");
}

std::shared_ptr<Obj> Continue::unaryop(Token *op) {
    UNIMPLEMENTED("Continue::unaryop");
}
