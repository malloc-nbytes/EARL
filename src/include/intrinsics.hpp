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

// File: intrinsics.hpp
// Description:
//   This file has the goal of containing all intrinsic functions
//   and or identifiers relevant to EARL.

#ifndef INTRINSICS_H
#define INTRINSICS_H

#include <unordered_map>

#include "interpreter.hpp"
#include "ctx.hpp"
#include "ast.hpp"

namespace Intrinsics {
  // Checks whether or not the string `id` is an intrinsic function
  // in the `intrinsic_functions` map.
  bool is_intrinsic_function(const std::string &id);

  // Runs the intrinsic function. It takes the contex `ctx` and the function
  // call expression `expr`.
  // NOTE: it is expected to run `is_intrinsic_function` before calling this function.
  Interpreter::ExprEvalResult run_intrinsic_function(ExprFuncCall *expr, Ctx &ctx);

  /*** INTRINSIC FUNCTION IMPLEMENTATIONS ***/

  // Given the context `ctx` and the function call expression `expr`
  // will print the values separated by a space, with the last one
  // not having that extra space. It is ended with a newline. It does
  // not flush the output.
  Interpreter::ExprEvalResult print(ExprFuncCall *expr, Ctx &ctx);

  using IntrinsicFunction = Interpreter::ExprEvalResult(*)(ExprFuncCall*, Ctx&);

  // A map of string -> function pointers. All intrinsics are expected
  // to have the signature of Interpreter::ExprEvalResult(*)(Ctx&, ExprFuncCall*).
  extern const std::unordered_map<std::string, IntrinsicFunction> intrinsic_functions;
};

#endif // INTRINSICS_H
