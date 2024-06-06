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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <any>
#include <variant>
#include <tuple>

#include "ctx.hpp"
#include "earlty.hpp"
#include "ast.hpp"

/// @brief The namespace for the interpreter during runtime
namespace Interpreter {

    /// @brief Used for an `ExprEvalResult` that shows
    /// the exact literal result that it was e.i, Token, Integer Literal etc..
    namespace LiteralResult {
        using Ident = EarlVar*;
        using Literal = Token*;
        using None = bool;

        struct Result {
            std::variant<Ident, Literal, None> m_value;
        };

    };

    /// @brief Struct that all expressions return.
    /// It contains information of the actual
    /// value of the expression as well as
    /// the type of expression that was evaluated.
    struct ExprEvalResult {
        /// @brief The actual evaluated result.
        /// <int, float, std::string, ... etc.>
        std::any m_expr_value;

        /// What kind of term did we encounter?
        /// Integer? Identifier? etc...
        ExprTermType m_expr_term_type;

        /// @brief The type that was returned as an EARL type
        EarlTy::Type m_earl_type;

        /// @brief The literal value of the evaluated result
        LiteralResult::Result m_literal_result;

        /// @brief Given a valid ExprEvalResult, returns the
        /// type as EarlTy::Type. If the type of the
        /// expression is an identifier, it will do a
        /// lookup in the scope to find the type of the
        /// already stored variable.
        EarlTy::Type get_earl_type(Ctx &ctx);

        /// @brief Get the inner value of the `ExprEvalResult`
        std::any value(void);

    };

    ExprEvalResult interpret(Program &program);
    ExprEvalResult eval_expr(Expr *expr, Ctx &ctx);
};

#endif // INTERPRETER_H
