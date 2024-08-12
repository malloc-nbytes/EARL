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
#include <vector>
#include <iostream>
#include <memory>

#include "repl.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include "interpreter.hpp"
#include "intrinsics.hpp"
#include "err.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "ctx.hpp"
#include "common.hpp"
#include "earl.hpp"
#include "lexer.hpp"

std::shared_ptr<Ctx>
Repl::run(void) {
    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types    = {};
    std::string comment               = COMMON_EARL_COMMENT;

    std::shared_ptr<Ctx> ctx = std::make_shared<WorldCtx>();

    while (true) {
        std::string line;
        std::vector<std::string> lines;

        int brace = 0;
        int bracket = 0;

        while (1) {
            std::cout << ">>> ";
            std::getline(std::cin, line);
            switch (line.back()) {
            case '{': ++brace; break;
            case '}': --brace; break;
            case '[': ++bracket; break;
            case ';': {
                // closure 1
                if (line.size() > 1 && line[line.size()-2] == '}')
                    --brace;
                // closure 2
                if (line.size() > 2 && line[line.size()-3] == '}')
                    --brace;
                // list
                if (line.size() > 1 && line[line.size()-2] == ']')
                    --bracket;
            } break;
            default: break;
            }
            if (line == "" && brace == 0 && bracket == 0)
                break;
            lines.push_back(line);
        }

        std::string combined = "";
        std::for_each(lines.begin(), lines.end(), [&](auto &s) {combined += s; });

        std::unique_ptr<Lexer> lexer = lex_file(combined.c_str(), "", keywords, types, comment);
        std::unique_ptr<Program> program = Parser::parse_program(*lexer.get());

        WorldCtx *wctx = dynamic_cast<WorldCtx*>(ctx.get());
        wctx->add_repl_lexer(std::move(lexer));
        wctx->add_repl_program(std::move(program));

        for (size_t i = 0; i < wctx->stmts_len(); ++i) {
            Stmt *stmt = wctx->stmt_at(i);
            if (!stmt->m_evald) {
                auto val = Interpreter::eval_stmt(stmt, ctx);
                if (val) {
                    std::vector<std::shared_ptr<earl::value::Obj>> params = {val};
                    (void)Intrinsics::intrinsic_print(params, ctx);
                    std::cout << " -> " << earl::value::type_to_str(val->type()) << std::endl;
                }
            }
        }
    }

    return nullptr;
}

