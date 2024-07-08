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

#include "intrinsics.hpp"
#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

earl::value::Obj *Intrinsics::intrinsic_member_split(earl::value::Obj *obj, std::vector<earl::value::Obj *> &delim, Ctx &ctx) {
    (void)obj;
    (void)delim;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_split");
}

earl::value::Obj *Intrinsics::intrinsic_member_substr(earl::value::Obj *obj, std::vector<earl::value::Obj *> &idxs, Ctx &ctx) {
    (void)ctx;
    (void)obj;
    (void)idxs;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_substr");
}

earl::value::Obj *Intrinsics::intrinsic_member_remove_lines(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)obj;
    (void)unused;
    (void)ctx;
    UNIMPLEMENTED("Intrinsics::intrinsic_member_remove_lines");
}
