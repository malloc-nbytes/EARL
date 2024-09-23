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
#include <ctime>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Time::Time(void) : m_now(std::time(nullptr)) {}

Time::Time(std::time_t now) : m_now(now) {}

std::shared_ptr<Tuple>
Time::pretty(void) {
    std::tm *local = std::localtime(&m_now);
    int year = local->tm_year + 1900,
        month = local->tm_mon + 1,
        day = local->tm_mday,
        hour = local->tm_hour,
        min = local->tm_min,
        second = local->tm_sec;
    std::vector<std::shared_ptr<Obj>> values = {
        std::make_shared<Int>(year),
        std::make_shared<Int>(month),
        std::make_shared<Int>(day),
        std::make_shared<Int>(hour),
        std::make_shared<Int>(min),
        std::make_shared<Int>(second),
    };
    return std::make_shared<Tuple>(std::move(values));
}

// Implements
Type
Time::type(void) const {
    return Type::Time;
}

void Time::mutate(Obj *other, StmtMut *stmt) {
    UNIMPLEMENTED("Time::mutate");
}

std::shared_ptr<Obj>
Time::copy(void) {
    return std::make_shared<Time>(m_now);
}

bool
Time::eq(Obj *other) {
    UNIMPLEMENTED("Time::eq");
}

std::string
Time::to_cxxstring(void) {
    return std::to_string(m_now);
}

std::shared_ptr<Obj>
Time::add(Token *op, Obj *other) {
    UNIMPLEMENTED("Time::add");
}

std::shared_ptr<Obj>
Time::sub(Token *op, Obj *other) {
    UNIMPLEMENTED("Time::sub");
}

std::shared_ptr<Obj>
Time::gtequality(Token *op, Obj *other) {
    UNIMPLEMENTED("Time::gtequality");
}

std::shared_ptr<Obj>
Time::equality(Token *op, Obj *other) {
    UNIMPLEMENTED("Time::equality");
}
