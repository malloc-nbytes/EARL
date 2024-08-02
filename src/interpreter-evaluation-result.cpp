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

#include <memory>

#include "interpreter.hpp"
#include "ctx.hpp"
#include "earl.hpp"

Interpreter::ER::ER(std::shared_ptr<earl::value::Obj> value,
                    ERT rt,
                    std::string id,
                    void *extra,
                    std::shared_ptr<Ctx> ctx) {
    this->value = value;
    this->rt = static_cast<uint32_t>(rt);
    this->id = id;
    this->extra = extra;
    this->ctx = ctx;
}

bool
Interpreter::ER::is_literal(void) {
    return (this->rt & ERT::Literal) != 0;
}

bool
Interpreter::ER::is_ident(void) {
    return (this->rt & ERT::Ident) != 0;
}

bool
Interpreter::ER::is_intrinsic(void) {
    return (this->rt & ERT::IntrinsicFunction) != 0;
}

bool
Interpreter::ER::is_function_ident(void) {
    return (this->rt & ERT::FunctionIdent) != 0;
}

bool
Interpreter::ER::is_class_instant(void) {
    return (this->rt & ERT::ClassInstant) != 0;
}

bool
Interpreter::ER::is_member_intrinsic(void) {
    return (this->rt & ERT::IntrinsicMemberFunction) != 0;
}

bool
Interpreter::ER::is_none(void) {
    return (this->rt & ERT::None) != 0;
}
