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

#include <filesystem>
#include <iostream>
#include <vector>
#include <iostream>

#include "err.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "lexer.hpp"
#include "interpreter.hpp"
#include "repl.hpp"
#include "ctx.hpp"
#include "config.h"
#include "hot-reload.hpp"

std::vector<std::string> earl_argv = {};
static std::vector<std::string> watch_files = {};

uint32_t flags = 0x00;

static void
usage(void) {
    std::cerr << "Bugs can be reported at <zdhdev@yahoo.com>" << std::endl;
    std::cerr << "or https://github.com/malloc-nbytes/EARL/issues" << std::endl << std::endl;

    std::cerr << "Documentation can be found on the Github repository: https://github.com/malloc-nbytes/EARL/" << std::endl;
    std::cerr << "  README                  -> compiling, installing, contributing" << std::endl;
    std::cerr << "  docs/html/index.html    -> source code (make docs)" << std::endl;
    std::cerr << "  EARL-language-reference -> how to use EARL" << std::endl << std::endl;

    std::cerr << "Usage: earl [options...] <file> -- [args...]" << std::endl << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -v, --version           Print version information" << std::endl;
    std::cerr << "  -h, --help              Print this help message" << std::endl;
    std::cerr << "      --without-stdlib    Do not use standard library" << std::endl;
    std::cerr << "      --repl-nocolor      Do not use color in the REPL" << std::endl;
    std::cerr << "      --watch [files...]  Watch files for changes and hot reload" << std::endl;

    std::exit(0);
}

static void
version() {
    std::cout << "EARL " << VERSION << std::endl;
    exit(0);
}

static void
gather_watch_files(std::vector<std::string> &args) {
    while (args.size() > 0) {
        if (!std::filesystem::exists(std::filesystem::path(args.at(0)))) {
            break;
        }
        watch_files.push_back(std::string(args.at(0)));
        args.erase(args.begin());
    }
}

static void
parse_2hypharg(std::string arg, std::vector<std::string> &args) {
    if (arg == COMMON_EARL2ARG_WITHOUT_STDLIB)
        flags |= __WITHOUT_STDLIB;
    else if (arg == COMMON_EARL2ARG_HELP)
        usage();
    else if (arg == COMMON_EARL2ARG_VERSION)
        version();
    else if (arg == COMMON_EARL2ARG_REPL_NOCOLOR)
        flags |= __REPL_NOCOLOR;
    else if (arg == COMMON_EARL2ARG_WATCH) {
        gather_watch_files(args);
        flags |= __WATCH;
    }
    else {
        ERR_WARGS(Err::Type::Fatal, "unrecognised argument `%s`", arg.c_str());
    }
}

static void
parse_1hypharg(std::string arg) {
    for (size_t i = 0; i < arg.size(); ++i) {
        switch (arg[i]) {
        case COMMON_EARL1ARG_HELP: {
            usage();
        } break;
        case COMMON_EARL1ARG_VERSTION: {
            version();
        } break;
        default: {
            ERR_WARGS(Err::Type::Fatal, "unrecognised argument `%c`", arg[i]);
        } break;
        }
    }
}

static void
parsearg(std::string line, std::vector<std::string> &args) {
    if (line.size() > 1 && line[0] == '-' && line[1] == '-') {
        args.erase(args.begin());
        parse_2hypharg(line.substr(2), args);
    }
    else if (line[0] == '-') {
        parse_1hypharg(line.substr(1));
    }
    else {
        UNIMPLEMENTED("parsearg: last case");
    }
}

static void
parse_earl_argv(std::vector<std::string> &args) {
    args.erase(args.begin());
    for (auto &s : args)
        earl_argv.push_back(s);
}

static std::string
handlecli(int argc, char **argv) {
    std::string filepath = "";
    std::vector<std::string> args = {};
    while (*argv) {
        args.push_back(std::string(*argv));
        ++argv;
    }

    int i = 0;
    while (args.size() > 0) {
        const std::string &entry = args.at(i);
        if (entry == "--") {
            parse_earl_argv(args);
            break;
        }
        if (entry[0] == '-') {
            parsearg(entry, args);
        }
        else {
            if (filepath != "")
                ERR(Err::Type::Fatal, "too many input files provided");
            filepath = entry;
            earl_argv.push_back(filepath);
            args.erase(args.begin());
        }
    }

    return filepath;
}

int
main(int argc, char **argv) {
    ++argv; --argc;
    std::string filepath = handlecli(argc, argv);

    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types = {};
    std::string comment = "#";

    if ((flags & __WATCH) != 0) {
        if (watch_files.size() == 0) {
            std::cerr << "Cannot use flag `" << COMMON_EARL2ARG_WATCH << "` with no watch files\n";
            std::exit(1);
        }
        hot_reload::register_watch_files(watch_files);
    }

    bool locked = true;

    if (filepath != "") {
        do {
            // No need to check for __WATCH cause this statement
            // will not happen unless we are looping, which is
            // already determined by __WATCH.
            if (!locked)
                hot_reload::watch();
            else
                locked = false;

            std::unique_ptr<Lexer> lexer = nullptr;
            std::unique_ptr<Program> program = nullptr;
            try {
                std::string src_code = read_file(filepath.c_str());
                lexer = lex_file(src_code, filepath, keywords, types, comment);
            } catch (const LexerException &e) {
                std::cerr << "Lexer error: " << e.what() << std::endl;
                if ((flags & __WATCH) == 0)
                    return 1;
            }
            try {
                program = Parser::parse_program(*lexer.get());
            } catch (const ParserException &e) {
                std::cerr << "Parser error: " << e.what() << std::endl;
                if ((flags & __WATCH) == 0)
                    return 1;
            }
            try {
                (void)Interpreter::interpret(std::move(program), std::move(lexer));
            } catch (const InterpreterException &e) {
                std::cerr << "Interpreter error: " << e.what() << std::endl;
                if ((flags & __WATCH) == 0)
                    return 1;
            }

        } while ((flags & __WATCH) != 0);
    }
    else {
        flags |= __REPL;
        std::cout << "EARL REPL v" << VERSION << '\n';
        std::cout << "Use `:help` for help" << std::endl;
        Repl::run();
    }

    return 0;
}
