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
#include <algorithm>
#include <vector>
#include <iostream>

#include "err.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "lexer.hpp"
#include "interpreter.hpp"

std::vector<std::string> earl_argv = {};

uint32_t flags = 0x00;

static void usage(void) {
    std::cerr << "Bugs can be reported at <zdhdev@yahoo.com>" << std::endl;
    std::cerr << "or https://github.com/malloc-nbytes/EARL/issues" << std::endl << std::endl;

    std::cerr << "Documentation can be found on the Github repository: https://github.com/malloc-nbytes/EARL/" << std::endl;
    std::cerr << "  README                  -> compiling, installing, contributing" << std::endl;
    std::cerr << "  docs/html/index.html    -> source code (make docs)" << std::endl;
    std::cerr << "  EARL-language-reference -> how to use EARL" << std::endl << std::endl;

    std::cerr << "Usage: earl [options] [file] -- <args>" << std::endl << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -v, --version         Print version information" << std::endl;
    std::cerr << "  -h, --help            Print this help message" << std::endl;
    std::cerr << "      --without-stdlib  Do not use standard library" << std::endl;
}

static void version() {
    std::cout << "EARL " << VERSION << std::endl;
}

static void parse_2hypharg(std::string arg) {
    if (arg == COMMON_EARL2ARG_WITHOUT_STDLIB) {
        flags |= __WITHOUT_STDLIB;
    }
    else if (arg == COMMON_EARL2ARG_HELP) {
        usage();
    }
    else if (arg == COMMON_EARL2ARG_VERSION) {
        version();
    }
    else {
        ERR_WARGS(Err::Type::Fatal, "unrecognised argument `%s`", arg.c_str());
    }
}

static void parse_1hypharg(std::string arg) {
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

static void parsearg(std::string line) {
    if (line.size() > 1 && line[0] == '-' && line[1] == '-') {
        parse_2hypharg(line.substr(2));
    }
    else if (line[0] == '-') {
        parse_1hypharg(line.substr(1));
    }
    else {
        UNIMPLEMENTED("parsearg: last case");
    }
}

static void parse_earl_argv(int i, int argc, char **argv) {
    for (i = i+1; i < argc; ++i) {
        earl_argv.push_back(std::string(argv[i]));
    }
}

static std::string handlecli(int argc, char **argv) {
    std::string filepath = "";

    for (int i = 0; i < argc; ++i) {
        std::string line = std::string(argv[i]);
        if (line == "--") {
            parse_earl_argv(i, argc, argv);
            break;
        }

        if (line[0] == '-') {
            parsearg(line);
        }
        else {
            if (filepath != "") {
                ERR(Err::Type::Fatal, "too many input files provided");
            }
            filepath = line;
            earl_argv.push_back(filepath);
        }
    }

    return filepath;
}

int main(int argc, char **argv) {
    if (argc == 1 || argc == 0) {
        usage();
    }

    ++argv; --argc;
    std::string filepath = handlecli(argc, argv);

    if (filepath != "") {
        std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
        std::vector<std::string> types = {};
        std::string comment = "#";

        std::unique_ptr<Lexer> lexer = lex_file(filepath.c_str(), keywords, types, comment);
        std::unique_ptr<Program> program = Parser::parse_program(*lexer.get());
        Interpreter::interpret(std::move(program), std::move(lexer));
    }

    return 0;
}

