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

#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

/**
 * A few things that are useful throughout the entire project.
 */

static std::string __internal_cwd = "";
extern int g_silence_exceptions;

namespace config {
    namespace prelude {
        namespace watch {
            extern std::vector<std::string> files;
            extern size_t run_count;
        };
        namespace one_shot {
            extern std::string code;
        };
        namespace time {
            extern std::chrono::time_point<std::chrono::high_resolution_clock> start;
            extern std::chrono::time_point<std::chrono::high_resolution_clock> end;
        };
        namespace include {
            extern std::vector<std::string> dirs;
        };
        namespace import {
            extern std::vector<std::string> dirs;
        };
        namespace topy {
            extern std::string formatter;
            extern std::string output;
        };
        extern std::vector<std::string> scripts;
    };

    namespace runtime {
        extern std::vector<std::string> argv;
        extern uint32_t flags;
        extern std::unordered_map<std::string, std::string> persistent_mem;
    };

    namespace repl {
        namespace welcome {
            extern std::string msg;
        };
        namespace theme {
            extern std::vector<std::string> available;
            extern std::string selected;
        };
    };
};

// extern uint32_t flags;

extern std::string repl_welcome_msg;

#define __WITHOUT_STDLIB           1 << 0
#define __REPL                     1 << 1
#define __REPL_NOCOLOR             1 << 2
#define __WATCH                    1 << 3
#define __SHOWFUNS                 1 << 4
#define __CHECK                    1 << 5
#define __TOPY                     1 << 6
#define __VERBOSE                  1 << 7
#define __SHOWBASH                 1 << 8
#define __SHOWLETS                 1 << 9
#define __SHOWMUTS                 1 << 10
#define __NO_SANITIZE_PIPES        1 << 11
#define __ERROR_ON_BASH_FAIL       1 << 12
#define __SUPPRESS_WARNINGS        1 << 13
#define __DISABLE_IMPLICIT_RETURNS 1 << 14
#define __ONE_SHOT                 1 << 15
#define __TIME                     1 << 16
#define __CLEAR_MEM_FILE           1 << 17

#define COMMON_EARL2ARG_HELP                     "help"
#define COMMON_EARL2ARG_WITHOUT_STDLIB           "without-stdlib"
#define COMMON_EARL2ARG_VERSION                  "version"
#define COMMON_EARL2ARG_REPL_NOCOLOR             "repl-nocolor"
#define COMMON_EARL2ARG_WATCH                    "watch"
#define COMMON_EARL2ARG_SHOWFUNS                 "show-funs"
#define COMMON_EARL2ARG_CHECK                    "check"
#define COMMON_EARL2ARG_TOPY                     "to-py"
#define COMMON_EARL2ARG_VERBOSE                  "verbose"
#define COMMON_EARL2ARG_INSTALL_PREFIX           "install-prefix"
#define COMMON_EARL2ARG_SHOWBASH                 "print-bash-execution"
#define COMMON_EARL2ARG_ERROR_ON_BASH_FAIL       "error-on-bash-fail"
#define COMMON_EARL2ARG_SHOWLETS                 "show-lets"
#define COMMON_EARL2ARG_SHOWMUTS                 "show-muts"
#define COMMON_EARL2ARG_NO_SANITIZE_PIPES        "no-sanitize-pipes"
#define COMMON_EARL2ARG_INCLUDE                  "include"
#define COMMON_EARL2ARG_IMPORT                   "import"
#define COMMON_EARL2ARG_REPL_THEME               "repl-theme"
#define COMMON_EARL2ARG_CREATE_DEF_CONF          "create-default-config"
#define COMMON_EARL2ARG_BATCH                    "batch"
#define COMMON_EARL2ARG_SUPPRESS_WARNINGS        "suppress-warnings"
#define COMMON_EARL2ARG_DISABLE_IMPLICIT_RETURNS "disable-implicit-returns"
#define COMMON_EARL2ARG_ONE_SHOT                 "oneshot"
#define COMMON_EARL2ARG_PORTABLE                 "is-portable"
#define COMMON_EARL2ARG_REPL_WELCOME             "repl-welcome"
#define COMMON_EARL2ARG_TIME                     "time"
#define COMMON_EARL2ARG_CLEAR_MEM_FILE           "clear-mem"

#define COMMON_EARL2ARG_ASCPL {                         \
        COMMON_EARL2ARG_HELP,                           \
            COMMON_EARL2ARG_WITHOUT_STDLIB,             \
            COMMON_EARL2ARG_VERSION,                    \
            COMMON_EARL2ARG_REPL_NOCOLOR,               \
            COMMON_EARL2ARG_WATCH,                      \
            COMMON_EARL2ARG_SHOWFUNS,                   \
            COMMON_EARL2ARG_CHECK,                      \
            COMMON_EARL2ARG_TOPY,                       \
            COMMON_EARL2ARG_VERBOSE,                    \
            COMMON_EARL2ARG_INSTALL_PREFIX,             \
            COMMON_EARL2ARG_SHOWBASH,                   \
            COMMON_EARL2ARG_SHOWLETS,                   \
            COMMON_EARL2ARG_SHOWMUTS,                   \
            COMMON_EARL2ARG_NO_SANITIZE_PIPES,          \
            COMMON_EARL2ARG_INCLUDE,                    \
            COMMON_EARL2ARG_IMPORT,                     \
            COMMON_EARL2ARG_REPL_THEME,                 \
            COMMON_EARL2ARG_CREATE_DEF_CONF,            \
            COMMON_EARL2ARG_BATCH,                      \
            COMMON_EARL2ARG_ERROR_ON_BASH_FAIL,         \
            COMMON_EARL2ARG_SUPPRESS_WARNINGS,          \
            COMMON_EARL2ARG_DISABLE_IMPLICIT_RETURNS,   \
            COMMON_EARL2ARG_ONE_SHOT,                   \
            COMMON_EARL2ARG_PORTABLE,                   \
            COMMON_EARL2ARG_REPL_WELCOME,               \
            COMMON_EARL2ARG_TIME                        \
            COMMON_EARL2ARG_CLEAR_MEM_FILE              \
            }

#define COMMON_EARL1ARG_HELP               'h'
#define COMMON_EARL1ARG_VERSION            'v'
#define COMMON_EARL1ARG_CHECK              'c'
#define COMMON_EARL1ARG_WATCH              'w'
#define COMMON_EARL1ARG_INCLUDE            'I'
#define COMMON_EARL1ARG_IMPORT             'i'
#define COMMON_EARL1ARG_BATCH              'b'
#define COMMON_EARL1ARG_VERBOSE            'V'
#define COMMON_EARL1ARG_SHOW_BASH          'x'
#define COMMON_EARL1ARG_ERROR_ON_BASH_FAIL 'e'
#define COMMON_EARL1ARG_SUPPRESS_WARNINGS  'S'
#define COMMON_EARL1ARG_ONE_SHOT           'O'

#define COMMON_EARL1ARG_ASCPL {                 \
        COMMON_EARL1ARG_HELP,                   \
            COMMON_EARL1ARG_VERSION,            \
            COMMON_EARL1ARG_CHECK,              \
            COMMON_EARL1ARG_WATCH,              \
            COMMON_EARL1ARG_INCLUDE,            \
            COMMON_EARL1ARG_IMPORT,             \
            COMMON_EARL1ARG_BATCH,              \
            COMMON_EARL1ARG_VERBOSE,            \
            COMMON_EARL1ARG_SHOW_BASH,          \
            COMMON_EARL1ARG_ERROR_ON_BASH_FAIL, \
            COMMON_EARL1ARG_SUPPRESS_WARNINGS,  \
            COMMON_EARL1ARG_ONE_SHOT            \
            }

void handle_2flag(std::string flag, bool set);
void handle_1flag(char flag, bool set);

#define COMMON_EARLATTR_WORLD        "world"
#define COMMON_EARLATTR_PUB          "pub"
#define COMMON_EARLATTR_REF          "ref"
#define COMMON_EARLATTR_CONST        "const"
#define COMMON_EARLATTR_EXPERIMENTAL "experimental"

#define COMMON_EARLATTR_ASCPL {                 \
        COMMON_EARLATTR_WORLD,                  \
            COMMON_EARLATTR_PUB,                \
            COMMON_EARLATTR_REF,                \
            COMMON_EARLATTR_CONST,              \
            COMMON_EARLATTR_EXPERIMENTAL        \
            }

#define COMMON_EARL_REPL_THEME_DEFAULT       "default"
#define COMMON_EARL_REPL_THEME_AQUA          "aqua"
#define COMMON_EARL_REPL_THEME_WHITEBOARD    "whiteboard"
#define COMMON_EARL_REPL_THEME_HALLOWEEN     "halloween"
#define COMMON_EARL_REPL_THEME_CHERRYBLOSSOM "cherryblossom"
#define COMMON_EARL_REPL_THEME_COLORBLIND    "colorblind"

#define COMMON_EARL_REPL_THEME_ASCPL {                  \
        COMMON_EARL_REPL_THEME_DEFAULT,                 \
            COMMON_EARL_REPL_THEME_AQUA,                \
            COMMON_EARL_REPL_THEME_WHITEBOARD,          \
            COMMON_EARL_REPL_THEME_HALLOWEEN,           \
            COMMON_EARL_REPL_THEME_CHERRYBLOSSOM,       \
            COMMON_EARL_REPL_THEME_COLORBLIND           \
            }

enum class Attr {
    World = 1 << 0,
    Pub = 1 << 1,
    Ref = 1 << 2,
    Const = 1 << 3,
    Experimental = 1 << 4,
};

// Keywords
#define COMMON_EARLKW_LET      "let"
#define COMMON_EARLKW_FN       "fn"
#define COMMON_EARLKW_RETURN   "return"
#define COMMON_EARLKW_IF       "if"
#define COMMON_EARLKW_ELSE     "else"
#define COMMON_EARLKW_WHILE    "while"
#define COMMON_EARLKW_FOR      "for"
#define COMMON_EARLKW_FOREACH  "foreach"
#define COMMON_EARLKW_IN       "in"
#define COMMON_EARLKW_IMPORT   "import"
#define COMMON_EARLKW_MODULE   "module"
#define COMMON_EARLKW_CLASS    "class"
#define COMMON_EARLKW_TRUE     "true"
#define COMMON_EARLKW_FALSE    "false"
#define COMMON_EARLKW_NONE     "none"
#define COMMON_EARLKW_MATCH    "match"
#define COMMON_EARLKW_WHEN     "when"
#define COMMON_EARLKW_BREAK    "break"
#define COMMON_EARLKW_ENUM     "enum"
#define COMMON_EARLKW_ALMOST   "almost"
#define COMMON_EARLKW_FULL     "full"
#define COMMON_EARLKW_AS       "as"
#define COMMON_EARLKW_TO       "to"
#define COMMON_EARLKW_CONTINUE "continue"
#define COMMON_EARLKW_LOOP     "loop"
#define COMMON_EARLKW_USE      "use"
#define COMMON_EARLKW_EXEC     "exec"
#define COMMON_EARLKW_CASE     "case"
#define COMMON_EARLKW_OF       "of"
#define COMMON_EARLKW_WITH     "with"
#define COMMON_EARLKW_TRY      "try"
#define COMMON_EARLKW_CATCH    "catch"

#define COMMON_EARLKW_ASCPL {                   \
        COMMON_EARLKW_LET,                      \
            COMMON_EARLKW_FN,                   \
            COMMON_EARLKW_RETURN,               \
            COMMON_EARLKW_IF,                   \
            COMMON_EARLKW_ELSE,                 \
            COMMON_EARLKW_WHILE,                \
            COMMON_EARLKW_FOR,                  \
            COMMON_EARLKW_FOREACH,              \
            COMMON_EARLKW_IN,                   \
            COMMON_EARLKW_IMPORT,               \
            COMMON_EARLKW_MODULE,               \
            COMMON_EARLKW_CLASS,                \
            COMMON_EARLKW_TRUE,                 \
            COMMON_EARLKW_FALSE,                \
            COMMON_EARLKW_NONE,                 \
            COMMON_EARLKW_MATCH,                \
            COMMON_EARLKW_WHEN,                 \
            COMMON_EARLKW_BREAK,                \
            COMMON_EARLKW_ENUM,                 \
            COMMON_EARLKW_ALMOST,               \
            COMMON_EARLKW_FULL,                 \
            COMMON_EARLKW_AS,                   \
            COMMON_EARLKW_TO,                   \
            COMMON_EARLKW_CONTINUE,             \
            COMMON_EARLKW_LOOP,                 \
            COMMON_EARLKW_USE,                  \
            COMMON_EARLKW_EXEC,                 \
            COMMON_EARLKW_CASE,                 \
            COMMON_EARLKW_OF,                   \
            COMMON_EARLKW_WITH,                 \
            COMMON_EARLKW_TRY,                  \
            COMMON_EARLKW_CATCH,                \
            }

// Types
#define COMMON_EARLTY_INT32   "int"
#define COMMON_EARLTY_FLOAT   "float"
#define COMMON_EARLTY_STR     "str"
#define COMMON_EARLTY_UNIT    "unit"
#define COMMON_EARLTY_CHAR    "char"
#define COMMON_EARLTY_BOOL    "bool"
#define COMMON_EARLTY_LIST    "list"
#define COMMON_EARLTY_TUPLE   "tuple"
#define COMMON_EARLTY_FILE    "file"
#define COMMON_EARLTY_CLOSURE "closure"
#define COMMON_EARLTY_OPTION  "option"
#define COMMON_EARLTY_SLICE   "slice"
#define COMMON_EARLTY_DICT    "dictionary"
#define COMMON_EARLTY_TYPE    "type"
#define COMMON_EARLTY_REAL    "real"
#define COMMON_EARLTY_ANY     "any"

#define COMMON_EARLTY_ASCPL {                   \
        COMMON_EARLTY_INT32,                    \
            COMMON_EARLTY_FLOAT,                \
            COMMON_EARLTY_STR,                  \
            COMMON_EARLTY_UNIT,                 \
            COMMON_EARLTY_CHAR,                 \
            COMMON_EARLTY_BOOL,                 \
            COMMON_EARLTY_LIST,                 \
            COMMON_EARLTY_TUPLE,                \
            COMMON_EARLTY_FILE,                 \
            COMMON_EARLTY_CLOSURE,              \
            COMMON_EARLTY_OPTION,               \
            COMMON_EARLTY_SLICE,                \
            COMMON_EARLTY_DICT,                 \
            COMMON_EARLTY_TYPE,                 \
            COMMON_EARLTY_REAL,                 \
            COMMON_EARLTY_ANY                   \
            }

#define COMMON_EARL_COMMENT "#"

#define COMMON_DEPTH_FULL 1
#define COMMON_DEPTH_ALMOST 0

#endif // COMMON_H
