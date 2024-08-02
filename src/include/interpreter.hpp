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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <memory>

#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"

/// @brief The namespace for the interpreter during runtime
namespace Interpreter {
    enum ERT {
        Literal = 1 << 0,
        Ident = 1 << 1,
        IntrinsicFunction = 1 << 2,
        IntrinsicMemberFunction = 1 << 3,
        FunctionIdent = 1 << 4,
        ClassInstant = 1 << 5,
        FromMemberAccess = 1 << 6,
        Wildcard = 1 << 7,
        None = 1 << 8,
    };

    struct ER {
        ER(std::shared_ptr<earl::value::Obj> value,
           ERT rt,
           std::string id = "",
           void *extra = nullptr,
           std::shared_ptr<Ctx> ctx = nullptr);

        bool is_literal(void);
        bool is_ident(void);
        bool is_intrinsic(void);
        bool is_function_ident(void);
        bool is_class_instant(void);
        bool is_member_intrinsic(void);
        bool is_wildcard(void);
        bool is_none(void);

        std::shared_ptr<earl::value::Obj> value;
        uint32_t rt;
        std::string id;
        void *extra;
        std::shared_ptr<Ctx> ctx;
    };

    std::shared_ptr<Ctx> interpret(std::unique_ptr<Program> program, std::unique_ptr<Lexer> lexer);
    ER eval_expr(Expr *expr, std::shared_ptr<Ctx> &ctx, bool ref);
    std::shared_ptr<earl::value::Obj> eval_stmt_block(StmtBlock *block, std::shared_ptr<Ctx> &ctx);
    std::shared_ptr<earl::value::Obj> eval_stmt(Stmt *stmt, std::shared_ptr<Ctx> &ctx);
};

#endif // INTERPRETER_H
