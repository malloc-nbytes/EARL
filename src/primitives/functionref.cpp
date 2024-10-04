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

#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

FunctionRef::FunctionRef(std::shared_ptr<earl::function::Obj> fun) : m_fun(fun) {}

std::shared_ptr<earl::function::Obj>
FunctionRef::value(void) {
    return m_fun->copy();
}

const std::vector<std::string> &
FunctionRef::get_info(void) {
    return m_fun->info();
}

// Implements
Type
FunctionRef::type(void) const {
    return Type::FunctionRef;
}

std::string
FunctionRef::to_cxxstring(void) {
    return "<Function Reference { src = "+m_fun->id()+" }>";
}

void
FunctionRef::mutate(Obj *other, StmtMut *stmt) {
    UNIMPLEMENTED("FunctionRef::mutate");
}

std::shared_ptr<Obj>
FunctionRef::copy(void) {
    return std::make_shared<FunctionRef>(m_fun);
}
