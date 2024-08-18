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

#include <algorithm>
#include <cassert>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

template <typename T>
void Dict<T>::insert(T key, std::shared_ptr<Obj> value) {
    assert(false);
}

/*** OVERRIDES ***/
template <typename T>
Type Dict<T>::type(void) const {
    UNIMPLEMENTED("Dict::type");
}

template <typename T>
std::shared_ptr<Obj> Dict<T>::binop(Token *op, std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Dict::binop");
}

template <typename T>
bool Dict<T>::boolean(void) {
    UNIMPLEMENTED("Dict::boolean");
}

template <typename T>
void Dict<T>::mutate(const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    UNIMPLEMENTED("Dict::mutate");
}

template <typename T>
std::shared_ptr<Obj> Dict<T>::copy(void) {
    UNIMPLEMENTED("Dict::copy");
}

template <typename T>
bool Dict<T>::eq(std::shared_ptr<Obj> &other) {
    UNIMPLEMENTED("Dict::eq");
}

template <typename T>
std::string Dict<T>::to_cxxstring(void) {
    UNIMPLEMENTED("Dict::to_cxxstring");
}

template <typename T>
void Dict<T>::spec_mutate(Token *op, const std::shared_ptr<Obj> &other, StmtMut *stmt) {
    UNIMPLEMENTED("Dict::spec_mutate");
}

template <typename T>
std::shared_ptr<Obj> Dict<T>::unaryop(Token *op) {
    UNIMPLEMENTED("Dict::unaryop");
}


