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
#include "config.h"
#include "hot-reload.hpp"
#include "earl-to-py.hpp"
#include "hidden-file.hpp"

static std::vector<std::string> scripts = {};
std::vector<std::string> earl_argv = {};
std::vector<std::string> watch_files = {};

// --repl-theme resources
std::vector<std::string> AVAILABLE_REPL_THEMES = COMMON_EARL_REPL_THEME_ASCPL;
std::string REPL_THEME = COMMON_EARL_REPL_THEME_DEFAULT;

// --include resources
std::vector<std::string> include_dirs = {};

// --import resources
std::vector<std::string> cli_import_dirs = {};

// --watch resources
static size_t run_count = 1;

// --to-py resources
static std::string to_py_formatter = "";
static std::string to_py_output = "";

uint32_t flags = 0x00;

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>

std::string
get_os_info() {
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    return "Windows " + osvi.dwMajorVersion + "."
        + osvi.dwMinorVersion + "(Build " + osvi.dwBuildNumber + ")";
}

#elif __linux__
#include <sys/utsname.h>

std::string
get_os_info() {
    struct utsname buffer;
    uname(&buffer);

    std::string sysname(buffer.sysname);
    std::string version(buffer.release);

    size_t hash_pos = version.find('#');
    if (hash_pos != std::string::npos)
        version = version.substr(0, hash_pos);

    return sysname + " " + version;
}

#elif __APPLE__
#include <sys/utsname.h>

std::string
get_os_info() {
    struct utsname buffer;
    uname(&buffer);
    std::string sysname(buffer.sysname);
    std::string release(buffer.release);
    return sysname + " " + release;
}

#else
std::string
get_os_info() {
    return "Unknown";
}
#endif

static void
usage(void) {
    std::cerr << "(MIT License) Copyright (c) 2023 malloc-nbytes" << std::endl << std::endl;
    std::cerr << "EARL v" << VERSION << ", " << COMPILER_INFO << " " << get_os_info() << std::endl << std::endl;

    std::cerr << "Bugs can be reported at <zdhdev@yahoo.com>" << std::endl;
    std::cerr << "or https://github.com/malloc-nbytes/EARL/issues" << std::endl << std::endl;

    std::cerr << "Documentation can be found at: https://malloc-nbytes.github.io/EARL-web/" << std::endl << std::endl;

    std::cerr << "Usage: earl [script (max 1 file)] [options...] -- [passed_args...]" << std::endl;
    std::cerr << "|  or: provide no script to launch the REPL" << std::endl << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "    Query Flags" << std::endl;
    std::cerr << "        -h, --help . . . . . . . . . . . . . . Print this help message" << std::endl;
    std::cerr << "        -v, --version  . . . . . . . . . . . . Print version information" << std::endl;
    std::cerr << "            --install-prefix . . . . . . . . . Print the installation prefix" << std::endl;
    std::cerr << "    Prelude Config" << std::endl;
    std::cerr << "        -c, --check  . . . . . . . . . . . . . Only parse the file given" << std::endl;
    std::cerr << "        -I, --include <DIR>  . . . . . . . . . Add an include directory" << std::endl;
    std::cerr << "        -i, --import <filepath>  . . . . . . . Import a file from the CLI" << std::endl;
    std::cerr << "        -w, --watch [files...] . . . . . . . . Watch files for changes and hot reload on save" << std::endl;
    std::cerr << "        -b  --batch [files...] . . . . . . . . Run multiple scripts in batch" << std::endl;
    std::cerr << "            --without-stdlib . . . . . . . . . Do not use standard library" << std::endl;
    std::cerr << "    Runtime Config" << std::endl;
    std::cerr << "            --verbose  . . . . . . . . . . . . Enable verbose mode" << std::endl;
    std::cerr << "            --show-funs  . . . . . . . . . . . Print every function call evaluated" << std::endl;
    std::cerr << "            --show-bash  . . . . . . . . . . . Print all inlined bash" << std::endl;
    std::cerr << "            --show-lets  . . . . . . . . . . . Print all variable instantiations" << std::endl;
    std::cerr << "            --show-muts  . . . . . . . . . . . Print all value mutations" << std::endl;
    std::cerr << "            --no-sanitize-pipes  . . . . . . . Do not sanitize bash pipes" << std::endl;
    std::cerr << "    REPL Config" << std::endl;
    std::cerr << "            --repl-nocolor . . . . . . . . . . Do not use color in the REPL" << std::endl;
    std::cerr << "            --repl-theme <theme>|list  . . . . Use a color theme in the REPL (`default` if this option is not used)" << std::endl;
    std::cerr << "                where" << std::endl;
    std::cerr << "                    list = list all available themes" << std::endl;
    std::cerr << "    Misc. Options" << std::endl;
    std::cerr << "            --create-default-config  . . . . . Create a default configuration file" << std::endl;
    std::cerr << "            --to-py output=O [formatter=F] . . Convert an EARL file to Python (experimental)" << std::endl;
    std::cerr << "                where" << std::endl;
    std::cerr << "                    O = stdout|<file>" << std::endl;
    std::cerr << "                    F = <program>" << std::endl;

    std::cerr << std::endl;

    std::cerr << "You can also create a configuration file by using the `--create-default-config` flag for a starting point." << std::endl << std::endl;

    std::cerr << "Examples:" << std::endl;
    std::cerr << "  earl                                   # Launch the REPL" << std::endl;
    std::cerr << "  earl --show-lets                       # Launch the REPL and print variable instantiations" << std::endl;
    std::cerr << "  earl --version                         # Show the version" << std::endl;
    std::cerr << "  earl --v                               # Show the version" << std::endl;
    std::cerr << "  earl script.earl --verbose --check     # Turn on verbose mode and check the file" << std::endl;
    std::cerr << "  earl script.earl -cw *.earl            # Check the file and hot reload on saving any EARL files" << std::endl;
    std::cerr << "  earl --batch script1.earl script2.earl # Run multiple EARL scripts" << std::endl;

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
        if (!std::filesystem::exists(std::filesystem::path(args.at(0))))
            break;
        watch_files.push_back(std::string(args.at(0)));
        args.erase(args.begin());
    }
}

static std::string
try_guess_wrong_arg(std::string &arg) {
    std::vector<std::string> possible = COMMON_EARL2ARG_ASCPL;
    int min = 1e9;
    int min_idx = 0;

    for (size_t i = 0; i < possible.size(); ++i) {
        int score = levenshtein_distance(arg, possible[i]);
        if (score < min) {
            min = score;
            min_idx = i;
        }
    }

    return possible.at(min_idx);
}

static void
handle_to_py_flag(std::vector<std::string> &args) {
    std::cout << "warning: flag `--" << COMMON_EARL2ARG_TOPY << "` is experimental and may not work correctly" << std::endl;
    flags |= __TOPY;
    while (args.size() != 0 && args[0][0] != '-') {
        const std::string &option = args.at(0);
        std::string left = "", right = "";
        size_t pos = option.find('=');
        if (pos != std::string::npos) {
            left = option.substr(0, pos);
            right = option.substr(pos+1);
            if (left == "formatter")
                to_py_formatter = right;
            else if (left == "output")
                to_py_output = right;
            else {
                std::cerr << "error: invalid option `" << left <<  "` for `--" << COMMON_EARL2ARG_TOPY << "`";
                std::exit(EXIT_FAILURE);
            }
            args.erase(args.begin());
        }
        else {
            std::cerr << "error: missing `=` in `--" << COMMON_EARL2ARG_TOPY << "` option" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

static void
install_prefix(void) {
    std::cout << "[EARL install-prefix] EARL and StdLib installed at " << PREFIX << std::endl;
    std::exit(0);
}

static void
add_import_file(std::vector<std::string> &args) {
    if (args.size() > 0) {
        cli_import_dirs.push_back(args.at(0));
        args.erase(args.begin());
    }
    else {
        std::cerr << "error: missing import file for flag `--" COMMON_EARL2ARG_IMPORT "`" << std::endl;
        std::exit(1);
    }
}

static void
assert_repl_theme_valid(void) {
    bool found = false;
    for (auto t : COMMON_EARL_REPL_THEME_ASCPL) {
        if (REPL_THEME == t) {
            found = true;
            break;
        }
    }
    if (!found) {
        std::cerr << "invalid theme: `" << REPL_THEME << "`" << std::endl;
        std::cerr << "see `--" << COMMON_EARL2ARG_REPL_THEME << " list` to view all themes" << std::endl;
        std::exit(1);
    }
}

static void
get_repl_theme(std::vector<std::string> &args) {
    if (args.size() > 0) {
        if (args[0] == "list") {
            std::cerr << "REPL Themes:" << std::endl;
            for (auto t : COMMON_EARL_REPL_THEME_ASCPL) {
                std::cerr << "| " << t;
                if (REPL_THEME == t)
                    std::cout << "        [selected]";
                std::cout << std::endl;
            }
            std::exit(0);
        }

        REPL_THEME = args.at(0);
        args.erase(args.begin());
        // Theme checking is done in main().
    }
    else {
        std::cerr << "error: missing theme for flag `--" COMMON_EARL2ARG_REPL_THEME "`" << std::endl;
        std::exit(1);
    }
}

static void
add_include_file(std::vector<std::string> &args) {
    if (args.size() > 0) {
        include_dirs.push_back(args.at(0));
        args.erase(args.begin());
    }
    else {
        std::cerr << "error: missing include directory for flag `--" COMMON_EARL2ARG_INCLUDE "`" << std::endl;
        std::exit(1);
    }
}

static void
get_batch_scripts(std::vector<std::string> &args) {
    while (args.size() > 0) {
        if (!std::filesystem::exists(std::filesystem::path(args.at(0))))
            break;
        scripts.push_back(std::string(args.at(0)));
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
    else if (arg == COMMON_EARL2ARG_SHOWFUNS)
        flags |= __SHOWFUNS;
    else if (arg == COMMON_EARL2ARG_CHECK)
        flags |= __CHECK;
    else if (arg == COMMON_EARL2ARG_TOPY)
        handle_to_py_flag(args);
    else if (arg == COMMON_EARL2ARG_VERBOSE)
        flags |= __VERBOSE;
    else if (arg == COMMON_EARL2ARG_INSTALL_PREFIX)
        install_prefix();
    else if (arg == COMMON_EARL2ARG_SHOWBASH)
        flags |= __SHOWBASH;
    else if (arg == COMMON_EARL2ARG_SHOWLETS)
        flags |= __SHOWLETS;
    else if (arg == COMMON_EARL2ARG_SHOWMUTS)
        flags |= __SHOWMUTS;
    else if (arg == COMMON_EARL2ARG_NO_SANITIZE_PIPES)
        flags |= __NO_SANITIZE_PIPES;
    else if (arg == COMMON_EARL2ARG_INCLUDE)
        add_include_file(args);
    else if (arg == COMMON_EARL2ARG_IMPORT)
        add_import_file(args);
    else if (arg == COMMON_EARL2ARG_REPL_THEME)
        get_repl_theme(args);
    else if (arg == COMMON_EARL2ARG_CREATE_DEF_CONF)
        create_default_config_file();
    else if (arg == COMMON_EARL2ARG_BATCH)
        get_batch_scripts(args);
    else {
        std::cerr << "error: Unrecognised argument: " << arg << std::endl;
        std::cerr << "Did you mean: " << try_guess_wrong_arg(arg) << "?" << std::endl;
        exit(1);
    }
}

static void
parse_1hypharg(std::string arg, std::vector<std::string> &args) {
    for (size_t i = 0; i < arg.size(); ++i) {
        switch (arg[i]) {
        case COMMON_EARL1ARG_HELP: {
            usage();
        } break;
        case COMMON_EARL1ARG_VERSION: {
            version();
        } break;
        case COMMON_EARL1ARG_CHECK: {
            flags |= __CHECK;
        } break;
        case COMMON_EARL1ARG_WATCH: {
            gather_watch_files(args);
            flags |= __WATCH;
        } break;
        case COMMON_EARL1ARG_INCLUDE: {
            add_include_file(args);
        } break;
        case COMMON_EARL1ARG_IMPORT: {
            add_import_file(args);
        } break;
        case COMMON_EARL1ARG_BATCH: {
            get_batch_scripts(args);
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
        args.erase(args.begin());
        parse_1hypharg(line.substr(1), args);
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

static void
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
            scripts.push_back(filepath);
        }
    }
}

int
main(int argc, char **argv) {
    ++argv; --argc;

    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types = {};
    std::string comment = "#";

    handle_hidden_file();
    assert_repl_theme_valid();
    handlecli(argc, argv);

    if (scripts.size() > 0) {
        for (auto &filepath : scripts) {
            if ((flags & __WATCH) != 0) {
                if (watch_files.size() == 0) {
                    std::cerr << "Cannot use flag `" << COMMON_EARL2ARG_WATCH << "` with no watch files\n";
                    std::exit(1);
                }
                hot_reload::register_watch_files(watch_files);
            }

            if ((flags & __TOPY) != 0) {
                std::unique_ptr<Lexer> lexer = nullptr;
                std::unique_ptr<Program> program = nullptr;
                try {
                    std::string src_code = read_file(filepath.c_str(), include_dirs);
                    lexer = lex_file(src_code, filepath, keywords, types, comment);
                } catch (const LexerException &e) {
                    std::cerr << "Lexer error: " << e.what() << std::endl;
                }
                try {
                    program = Parser::parse_program(*lexer.get(), filepath);
                } catch (const ParserException &e) {
                    std::cerr << "Parser error: " << e.what() << std::endl;
                }
                auto pysrc = earl_to_py(std::move(program));

                if (to_py_output == "") {
                    std::cerr << "[EARL] error: missing output filepath for flag `--" << COMMON_EARL2ARG_TOPY << "`" << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                if (to_py_output == "stdout")
                    std::cout << pysrc << std::endl;
                else {
                    std::ofstream pysrc_outfile(to_py_output);
                    if (!pysrc_outfile) {
                        std::cerr << "[EARL] error: opening file: " << to_py_output << std::endl;
                        std::exit(EXIT_FAILURE);
                    }
                    pysrc_outfile << pysrc;
                    pysrc_outfile.close();
                }

                if (to_py_formatter != "") {
                    std::string cmd = to_py_formatter+" "+to_py_output;
                    int exit_code = system(cmd.c_str());
                    if (exit_code != 0) {
                        std::cerr << "[EARL] error: formatting failed with code " << exit_code << std::endl;
                        std::exit(1);
                    }
                }

                std::exit(0);
            }

            if ((flags & __WATCH) != 0)
                std::cout << "[EARL] Now watching files and will hot reload on file save" << std::endl;

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

                    if ((flags & __WATCH) != 0)
                        std::cout << "=== Run: " << run_count++ << " ======================" << std::endl;

                    std::unique_ptr<Lexer> lexer = nullptr;
                    std::unique_ptr<Program> program = nullptr;
                    try {
                        std::string src_code = read_file(filepath.c_str(), include_dirs);
                        lexer = lex_file(src_code, filepath, keywords, types, comment);
                    } catch (const LexerException &e) {
                        std::cerr << "Lexer error: " << e.what() << std::endl;
                        if ((flags & __WATCH) == 0)
                            return 1;
                        continue;
                    }
                    try {
                        program = Parser::parse_program(*lexer.get(), filepath);
                    } catch (const ParserException &e) {
                        std::cerr << "Parser error: " << e.what() << std::endl;
                        if ((flags & __WATCH) == 0)
                            return 1;
                        continue;
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
        }
    }
    else {
        assert_repl_theme_valid();
        flags |= __REPL;
        std::cout << "EARL REPL v" << VERSION << '\n';
        std::cout << "Use `:help` for help and `:q` or C-c to quit" << std::endl;
        repl::run(include_dirs);
    }

    return 0;
}
