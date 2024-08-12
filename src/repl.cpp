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

#include <sstream>
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

#define GREEN "\033[32m"
#define GRAY "\033[90m"
#define RED "\033[31m"
#define NOC "\033[0m"

#define CANCEL ":c"
#define HELP ":help"
#define RM_LAST_ENTRY ":rm"
#define EDIT_ENTRY ":e"
#define LIST_ENTRIES ":ls"
#define IMPORT ":i"

void
log(std::string msg) {
    std::cout << "[EARL] " << msg << std::endl;
}

std::vector<std::string>
split_on_space(std::string &line) {
    std::vector<std::string> res = {};
    std::stringstream ss(line);
    std::string word;
    while (ss >> word)
        res.push_back(word);
    return res;
}

std::vector<std::string>
split_on_newline(std::string &line) {
    std::vector<std::string> res = {};
    std::string buf = "";
    for (auto &c : line) {
        if (c == '\n') {
            res.push_back(buf);
            buf = "";
        }
        else
            buf += c;
    }
    return res;
}

void
help(void) {
    log("(Help)");
    log("  " HELP " -> show this message");
    log("  " CANCEL " -> cancel current action");
    log("  " IMPORT " <file> -> import local EARL file");
    log("  " RM_LAST_ENTRY " -> remove previous entry");
    log("  " EDIT_ENTRY " [lineno...] -> edit previous entrie(s)");
    log("  " LIST_ENTRIES " -> list all entries in the current session");
}

void
import_file(std::vector<std::string> &args, std::vector<std::string> &lines) {
    if (args.size() == 0) {
        log("No file supplied");
        return;
    }
    else if (args.size() > 1)
        log("only expecting one argument, ignoring others...");
    const char *src_c = read_file(args[0].c_str());
    std::string src = std::string(src_c);
    auto src_lines = split_on_newline(src);
    std::for_each(src_lines.begin(), src_lines.end(), [&](auto &l){lines.push_back(l);});
}

std::string
get_special_input(void) {
    std::cout << ">>> ";
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void
rm_last_entry(std::vector<std::string> &entries) {
    if (entries.size() == 0)
        log("No previous entry exists");
    else
        entries.pop_back();
}

void
edit_entry(std::vector<std::string> &args, std::vector<std::string> &lines) {
    if (args.size() > 0) {
        for (auto arg : args) {
            int lnum = std::stoi(arg);
            if (lnum < 0 || lnum >= (int)lines.size()) {
                log("Line number is out of range for session");
                return;
            }
            log("Editing [ " RED + lines.at(lnum) + NOC " ]");
            std::string newline = get_special_input();
            if (newline == CANCEL) {
                log("Cancelling");
                return;
            }
            lines.at(lnum) = newline;
        }
    }
    else {
        if (lines.size() == 0) {
            log("No lines to edit");
            return;
        }
        log("Editing [ " RED + lines.back() + NOC " ]");
        std::string newline = get_special_input();
        lines.at(lines.size()-1) = newline;
    }
}

void
ls_entries(std::vector<std::string> &lines) {
    if (lines.size() == 0) {
        log("Nothing appropriate");
        return;
    }

    for (size_t i = 0; i < lines.size(); ++i)
        std::cout << i << ": " << lines[i] << std::endl;
}

void
handle_repl_arg(std::string &line, std::vector<std::string> &lines) {
    std::vector<std::string> lst = split_on_space(line);
    std::vector<std::string> args(lst.begin()+1, lst.end());

    if (lst[0] == RM_LAST_ENTRY)
        rm_last_entry(lines);
    else if (lst[0] == EDIT_ENTRY)
        edit_entry(args, lines);
    else if (lst[0] == LIST_ENTRIES)
        ls_entries(lines);
    else if (lst[0] == IMPORT) {
        import_file(args, lines);
    }
    else if (lst[0] == HELP)
        help();
    else
        log("unknown command sequence `" + lst[0] + "`");
}

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
        int quote = 0;
        int i = 0;

        while (1) {
            std::cout << i++ << ": ";
            std::getline(std::cin, line);
            if (line.size() > 0 && line[0] == ':') {
                handle_repl_arg(line, lines);
                --i;
            }
            else {
                switch (line.back()) {
                case '{': ++brace; break;
                case '}': --brace; break;
                case '[': ++bracket; break;
                case '"': ++quote; break;
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
                    // str
                    if (line.size() > 1 && line[line.size()-2] == '"')
                        --quote;
                } break;
                default: break;
                }
                if (line == "" && brace == 0 && bracket == 0)
                    break;
                lines.push_back(line);
            }
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
                    std::cout << GREEN;
                    (void)Intrinsics::intrinsic_print(params, ctx);
                    std::cout << GRAY;
                    std::cout << " -> " << earl::value::type_to_str(val->type()) << std::endl;
                    std::cout << NOC;
                }
            }
        }
    }

    return nullptr;
}

