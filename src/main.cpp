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

uint32_t flags = 0x00;

void usage(char *progname) {
    (void)progname;
    ERR_WARGS(Err::Type::Usage, "%s <input>", progname);
}

void parse_2hypharg(std::string arg) {
    if (arg == COMMON_EARL2ARG_WITHOUT_STDLIB) {
        flags |= __WITHOUT_STDLIB;
    }
    else if (arg == COMMON_EARL2ARG_VERSION) {
        std::cout << "EARL version " << VERSION << std::endl;
    }
    else {
        ERR_WARGS(Err::Type::Fatal, "unrecognised argument `%s`", arg.c_str());
    }
}

void parse_1hypharg(std::string arg) {
    for (size_t i = 0; i < arg.size(); ++i) {
        switch (arg[i]) {
        case COMMON_EARL1ARG_VERSTION: {
            std::cout << "EARL version " << VERSION << std::endl;
        } break;
        default: {
            ERR_WARGS(Err::Type::Fatal, "unrecognised argument `%c`", arg[i]);
        } break;
        }
    }
}

void parsearg(std::string line) {
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

const char *handlecli(int argc, char **argv) {
    const char *filepath = nullptr;

    for (int i = 0; i < argc; ++i) {
        const char *line = argv[i];
        if (line[0] == '-')
            parsearg(std::string(line));
        else
            filepath = line;
    }

    return filepath;
}

int main(int argc, char **argv) {
    if (argc == 1 || argc == 0) {
        usage(*argv);
    }

    const char *filepath = handlecli(argc, argv);
    // const char *filepath = *(++argv);

    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types = COMMON_EARLTY_ASCPL;
    std::string comment = "#";

    std::unique_ptr<Lexer> lexer = lex_file(filepath, keywords, types, comment);
    std::unique_ptr<Program> program = Parser::parse_program(*lexer.get());
    Interpreter::interpret(std::move(program), std::move(lexer));

    return 0;
}

