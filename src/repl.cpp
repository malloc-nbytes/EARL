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

#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <termios.h>

#include "repl.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include "intrinsics.hpp"
#include "err.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "ctx.hpp"
#include "common.hpp"
#include "earl.hpp"
#include "lexer.hpp"

#define EARL_REPL_HISTORY_FILENAME ".earl_history"
#define REPL_HISTORY_MAX_FILESZ 1024 * 1024

#define YELLOW "\033[93m"
#define GREEN "\033[32m"
#define GRAY "\033[90m"
#define RED "\033[31m"
#define BLUE "\033[94m"
#define NOC "\033[0m"

#define QUIT ":q"
#define CLEAR ":c"
#define SKIP ":skip"
#define HELP ":help"
#define RM_ENTRY ":rm"
#define EDIT_ENTRY ":e"
#define LIST_ENTRIES ":show"
#define IMPORT ":i"
#define DISCARD ":d"
#define EE ":q!"
#define VARS ":vars"
#define FUNCS ":funcs"
#define RESET ":reset"

static std::string REPL_HIST = "";

static int g_brace = 0;
static int g_bracket = 0;
static int g_paren = 0;

struct RawInput {
    RawInput() {
        tcgetattr(STDIN_FILENO, &old_termios);
        termios raw = old_termios;
        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    ~RawInput() {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    }

    char get_char() {
        char ch;
        read(STDIN_FILENO, &ch, 1);
        return ch;
    }

private:
    struct termios old_termios;
};

static RawInput RI;


#define ENTER(ch) (ch)=='\n'
#define BACKSPACE(ch) (ch) == 8 || (ch) == 127
#define ESCAPESEQ(ch) (ch) == 27
#define CSI(ch) (ch) == '['
#define TAB(ch) (ch) == '\t'
#define UP_ARROW 'A'
#define DOWN_ARROW 'B'
#define RIGHT_ARROW 'C'
#define LEFT_ARROW 'D'

void
clearln(bool flush=false) {
    std::cout << "\r" << std::string(64, ' ') << "\r";
    if (flush)
        std::cout.flush();
}

static size_t lineno = 0;

void
out_lineno(bool flush=false) {
    clearln();
    std::cout << lineno << ": ";
    if (flush)
        std::cout.flush();
}

void
handle_backspace(std::string prompt, char ch, int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    if (c <= 0)
        return;

    std::cout << "\033[D";
    std::cout.flush();

    line.erase(c-1, 1);

    clearln();
    std::cout << prompt << line;
    // std::cout << "\033[" << (line.size() - (c-1)) << "D";
    std::cout << "\033[" << c+pad << "G";
    --c;
    std::cout.flush();
}

void
handle_newline(int &lines_idx, std::string &line, std::vector<std::string> &lines) {
    std::cout << std::endl;
    if (line != "") {
        lines.push_back(line);
        line.clear();
        lines_idx = lines.size()-1;
    }
}

void handle_up_arrow(std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines) {
    if (lines.size() == 0)
        return;
    if (lines_idx <= 0)
        return;

    --lines_idx;
    std::string &histline = lines[lines_idx];
    clearln();
    std::cout << prompt << histline;
    line = histline;
    std::cout.flush();
}

void handle_down_arrow(std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines) {
    if (lines.size() == 0)
        return;
    if (lines_idx >= static_cast<int>(lines.size()) - 1)
        return;

    ++lines_idx;
    std::string &histline = lines[lines_idx];
    clearln();
    std::cout << prompt << histline;
    line = histline;
    std::cout.flush();
}

void
handle_left_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    if (c <= 0)
        return;
    --c;
    std::cout << "\033[D";
    std::cout.flush();
}

void
handle_right_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    if (c >= static_cast<int>(line.size()))
        return;
    ++c;
    std::cout << "\033[C";
    std::cout.flush();
}

void
handle_tab(int &c, std::string &line, std::vector<std::string> &lines) {
    std::cout << std::string(2, ' ');
    std::cout.flush();
    line.push_back(' ');
    line.push_back(' ');
    c += 2;
}

std::string
getln(std::string prompt, std::vector<std::string> &history) {
    const int PAD = prompt.size();
    std::string line = "";
    int lines_idx = history.size();
    int c = 0;

    std::cout << prompt;
    std::cout.flush();

    while (1) {
        char ch = RI.get_char();

        if (ENTER(ch))
            break;

        else if (TAB(ch))
            handle_tab(c, line, history);

        else if (ESCAPESEQ(ch)) {
            int next0 = RI.get_char();
            if (CSI(next0)) {
                int next1 = RI.get_char();
                switch (next1) {
                case UP_ARROW: {
                    handle_up_arrow(prompt, lines_idx, line, history);
                    c = line.size();
                    std::cout << "\033[" << PAD+c+1 << "G";
                } break;
                case DOWN_ARROW: {
                    handle_down_arrow(prompt, lines_idx, line, history);
                    c = line.size();
                    std::cout << "\033[" << PAD+c+1 << "G";
                } break;
                case RIGHT_ARROW: {
                    handle_right_arrow(c, PAD, line, history);
                } break;
                case LEFT_ARROW: {
                    handle_left_arrow(c, PAD, line, history);
                } break;
                default: break;
                }
            }
        }
        else if (BACKSPACE(ch)) {
            handle_backspace(prompt, ch, c, PAD, line, history);
        }
        else {
            if (c != line.size()) {
                clearln();
                line.insert(line.begin()+c, ch);
                std::cout << prompt;
                std::cout << line;
                std::cout << "\033[" << c+PAD+2 << "G";
            }
            else {
                line.push_back(ch);
                std::cout << ch;
            }
            ++c;
            std::cout.flush();
        }
    }

    return line;
}

void
try_clear_repl_history() {
    const char* home_dir = std::getenv("HOME");
    if (home_dir == nullptr) {
        std::cerr << "Unable to get home directory path. Will not clear REPL history." << std::endl;
        return;
    }

    std::string fp = std::string(home_dir) + "/" + EARL_REPL_HISTORY_FILENAME;

    std::ifstream file(fp, std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file for reading. Will not clear REPL history." << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    std::streampos filesz = file.tellg();
    file.close();

    if (filesz >= REPL_HISTORY_MAX_FILESZ) {
        std::ofstream file(fp, std::ios::trunc | std::ios::binary);
        if (!file) {
            std::cerr << "Unable to open file for clearing. Will not clear REPL history." << std::endl;
            return;
        }
        file.close();
        std::cout << "REPL history file cleared." << std::endl;
    }
}

void
yellow(void) {
    if ((flags & __REPL_NOCOLOR) == 0)
        std::cout << YELLOW;
}

void
blue(void) {
    if ((flags & __REPL_NOCOLOR) == 0)
        std::cout << BLUE;
}

void
green(void) {
    if ((flags & __REPL_NOCOLOR) == 0)
        std::cout << GREEN;
}

void
gray(void) {
    if ((flags & __REPL_NOCOLOR) == 0)
        std::cout << GRAY;
}

void
red(void) {
    if ((flags & __REPL_NOCOLOR) == 0)
        std::cout << RED;
}

void
noc(void) {
    if ((flags & __REPL_NOCOLOR) == 0)
        std::cout << NOC;
}

void
save_repl_history() {
    const char *home_dir = std::getenv("HOME");
    if (home_dir == nullptr)
        std::cerr << "Unable to get home directory path. Will not save REPL history." << std::endl;
    std::string fp = std::string(home_dir) + "/" EARL_REPL_HISTORY_FILENAME;
    std::ofstream of(fp, std::ios::app);
    if (!of)
        std::cerr << "Unable to open file for writing. Will not save REPL history." << std::endl;
    of << REPL_HIST;
    of.close();
}

void
log(std::string msg, void(*color)(void) = nullptr) {
    blue();
    std::cout << "[EARL] ";
    noc();
    if (color)
        color();
    std::cout << msg;
    noc();
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
    log(HELP " -> show this message\n");
    log(QUIT " -> quit the session\n");
    log(VARS " -> show current variables in scope\n");
    log(FUNCS " -> show current functions in scope\n");
    log(CLEAR " -> clear the screen\n");
    log(IMPORT " [files...] -> import local EARL files\n");
    log(RM_ENTRY " [lineno...] -> remove entries (previous if blank)\n");
    log(EDIT_ENTRY " [lineno...] -> edit previous entries (previous if blank)\n");
    log(LIST_ENTRIES " -> list all entries in the current session\n");
    log(DISCARD " -> discard the current session\n");
    log(RESET " -> reset all identifiers\n");
    log("You can also issue bash commands by typing `$` followed by the command\n");
}

void
import_file(std::vector<std::string> &args, std::vector<std::string> &lines) {
    if (args.size() == 0) {
        log("No files supplied\n");
        return;
    }
    for (auto &f : args) {
        const char *src_c = read_file(f.c_str());
        std::string src = std::string(src_c);
        auto src_lines = split_on_newline(src);
        std::for_each(src_lines.begin(), src_lines.end(), [&](auto &l){lines.push_back(l);});
        // green();
        log("Imported " + f + "\n", green);
        noc();
    }
}

std::string
get_special_input(void) {
    std::cout << ">>> ";
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void
analyze_new_line(std::string &line) {
    for (auto it = line.rbegin(); it != line.rend(); ++it) {
        switch (*it) {
        case '{': ++g_brace; break;
        case '}': --g_brace; break;
        case '[': ++g_bracket; break;
        case ']': --g_bracket; break;
        case '(': ++g_paren; break;
        case ')': --g_paren; break;
        case ';': break;
        default: break;
        }
    }
}

void
manage_removed_or_edited_line(std::string &line) {
    for (auto it = line.rbegin(); it != line.rend(); ++it) {
        switch (*it) {
        case '{': --g_brace; break;
        case '}': ++g_brace; break;
        case '[': --g_bracket; break;
        case ']': ++g_bracket; break;
        case '(': --g_paren; break;
        case ')': ++g_paren; break;
        }
    }
}

void
rm_entries(std::vector<std::string> &args, std::vector<std::string> &lines) {
    if (lines.size() == 0) {
        log("No previous entry exists\n");
        return;
    }

    std::vector<std::string> hist = {};

    if (args.size() == 0) {
        hist.push_back(lines.back());
        manage_removed_or_edited_line(lines.back());
        lines.pop_back();
    }
    else {
        int i = 0;
        for (auto &arg : args) {
            int lnum = i == 0 ? std::stoi(arg) : std::stoi(arg)-i;
            if (lnum < 0 || lnum >= (int)lines.size()) {
                log("Line number is out of range for session\n");
                return;
            }
            hist.push_back(lines[lnum]);
            manage_removed_or_edited_line(lines[lnum]);
            lines.erase(lines.begin()+lnum);
            ++i;
        }
    }

    log("Removed:\n");
    for (auto &h : hist) {
        red();
        std::cout << h;
        noc();
        std::cout << std::endl;
    }
}

void
edit_entry(std::vector<std::string> &args, std::vector<std::string> &lines) {
    std::cout << std::endl;
    log("`" SKIP "` to skip current selection or `" QUIT "` to cancel the session editing\n", gray);
    if (args.size() > 0) {
        for (auto arg : args) {
            int lnum = std::stoi(arg);
            if (lnum < 0 || lnum >= (int)lines.size()) {
                log("Line number is out of range for session\n");
                return;
            }
            log("Editing [", gray);
            yellow();
            std::cout << lines.at(lnum);
            gray();
            std::cout << "]" << std::endl;
            noc();
            std::string newline = get_special_input();
            if (newline == QUIT) {
                log("Quitting session editor\n", gray);
                return;
            }
            if (newline == SKIP)
                log("Skipping current selection\n", gray);
            else if (newline == "") {
                manage_removed_or_edited_line(lines.at(lnum));
                lines.erase(lines.begin()+lnum);
            }
            else {
                manage_removed_or_edited_line(lines.at(lnum));
                lines.at(lnum) = newline;
                analyze_new_line(lines.at(lnum));
            }
        }
    }
    else {
        if (lines.size() == 0) {
            log("No lines to edit\n", gray);
            return;
        }
        log("Editing [", gray);
        yellow();
        std::cout << lines.back();
        gray();
        std::cout << "]" << std::endl;
        noc();
        std::string newline = get_special_input();
        manage_removed_or_edited_line(lines.at(lines.size()-1));
        if (newline == "")
            lines.erase(lines.end()-1);
        else {
            lines.at(lines.size()-1) = newline;
            analyze_new_line(lines.at(lines.size()-1));
        }
    }
}

void
clearscrn(void) {
    if (system("clear") != 0)
        log("warning: failed to clearscrn\n", yellow);
}

void
ls_entries(std::vector<std::string> &lines) {
    if (lines.size() == 0) {
        log("Nothing appropriate\n", gray);
        return;
    }
    for (size_t i = 0; i < lines.size(); ++i) {
        green();
        std::cout << i << ": ";
        std::cout << lines[i];
        noc();
        std::cout << std::endl;
    }
}

void
bash(std::string &line) {
    std::string cmd = line.substr(1, line.size());
    if (system(cmd.c_str()) != 0)
        log("warning: failed to ls\n", yellow);
}

void
discard(std::vector<std::string> &lines) {
    auto to_lower = [](std::string &str) {
        for (char& c : str)
            c = std::tolower(static_cast<unsigned char>(c));
    };

    std::string line = "";
    while (true) {
        log("Discard the current session? [Y/n]: ", red);
        std::getline(std::cin, line);
        to_lower(line);
        if (line == "" || line == "y" || line == "yes") {
            lines.clear();
            return;
        }
        else if (line == "n" || line == "no")
            return;
        else
            log("invalid input\n");
    }
}

void
ee(void) {
    log("Hey! This is not vim! D:\n", red);
    log("Did you mean " QUIT "\n", gray);
}

void
show_vars(std::shared_ptr<Ctx> &ctx) {
    for (auto &v : ctx->get_available_variable_names())
        std::cout << v << std::endl;
}

void
show_funcs(std::shared_ptr<Ctx> &ctx) {
    for (auto &v : ctx->get_available_function_names())
        std::cout << v << std::endl;
}

void
reset(std::shared_ptr<Ctx> &ctx) {
    std::vector<std::string> vars = ctx->get_available_variable_names();
    std::vector<std::string> funcs = ctx->get_available_function_names();

    log("Removing: [", gray);

    for (int i = 0; i < vars.size(); ++i) {
        red();
        std::cout << vars[i];
        noc();
        if (i != vars.size()-1) {
            gray();
            std::cout << ", ";
            noc();
        }
    }

    if (funcs.size() > 0 && vars.size() > 0) {
        gray();
        std::cout << ", ";
        noc();
    }

    for (int i = 0; i < funcs.size(); ++i) {
        red();
        std::cout << funcs[i];
        noc();
        if (i != funcs.size()-1) {
            gray();
            std::cout << ", ";
            noc();
        }
    }

    gray();
    std::cout << "]" << std::endl;
    noc();

    ctx = std::make_shared<WorldCtx>();
}

void
handle_repl_arg(std::string &line, std::vector<std::string> &lines, std::shared_ptr<Ctx> &ctx) {
    std::vector<std::string> lst = split_on_space(line);
    std::vector<std::string> args(lst.begin()+1, lst.end());

    if (line.size() != 0 && line[0] == '$')
        bash(line);
    else if (lst[0] == RM_ENTRY)
        rm_entries(args, lines);
    else if (lst[0] == EDIT_ENTRY)
        edit_entry(args, lines);
    else if (lst[0] == LIST_ENTRIES)
        ls_entries(lines);
    else if (lst[0] == IMPORT)
        import_file(args, lines);
    else if (lst[0] == CLEAR)
        clearscrn();
    else if (lst[0] == QUIT)
        exit(0);
    else if (lst[0] == DISCARD)
        discard(lines);
    else if (lst[0] == EE)
        ee();
    else if (lst[0] == VARS)
        show_vars(ctx);
    else if (lst[0] == FUNCS)
        show_funcs(ctx);
    else if (lst[0] == RESET)
        reset(ctx);
    else if (lst[0] == HELP)
        help();
    else
        log("unknown command sequence `" + lst[0] + "`\n", gray);
}

std::shared_ptr<Ctx>
Repl::run(void) {
    std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
    std::vector<std::string> types    = {};
    std::string comment               = COMMON_EARL_COMMENT;

    std::shared_ptr<Ctx> ctx = std::make_shared<WorldCtx>();

    while (true) {
        std::vector<std::string> lines = {};
        while (true) {
            std::string prompt = std::to_string(lineno)+": ";
            auto line = getln(prompt, lines);
            if (line == "")
                break;
            else if (line[0] == ':') {
                clearln();
                handle_repl_arg(line, lines, ctx);
            }
            else {
                ++lineno;
                lines.push_back(line);
                std::cout << std::endl;
            }
        }

        clearln();
        std::string combined = "";

        std::for_each(lines.begin(), lines.end(), [&](auto &s) {combined += s+"\n";});
        REPL_HIST += combined;
        lines.clear();
        lineno = 0;

        std::unique_ptr<Program> program = nullptr;
        std::unique_ptr<Lexer> lexer = nullptr;
        lexer = lex_file(combined, "", keywords, types, comment);

        try {
            program = Parser::parse_program(*lexer.get());
        } catch (const ParserException &e) {
            std::cerr << "Parser error: " << e.what() << std::endl;
            continue;
        }

        WorldCtx *wctx = dynamic_cast<WorldCtx*>(ctx.get());
        wctx->add_repl_lexer(std::move(lexer));
        wctx->add_repl_program(std::move(program));

        for (size_t i = 0; i < wctx->stmts_len(); ++i) {
            Stmt *stmt = wctx->stmt_at(i);
            if (!stmt->m_evald) {
                try {
                    auto val = Interpreter::eval_stmt(stmt, ctx);
                    if (val) {
                        std::vector<std::shared_ptr<earl::value::Obj>> params = {val};
                        green();
                        (void)Intrinsics::intrinsic_print(params, ctx, nullptr);
                        std::cout << " -> ";
                        gray();
                        std::cout << earl::value::type_to_str(val->type()) << std::endl;
                        noc();
                    }
                }
                catch (InterpreterException &e) {
                    std::cerr << "Interpreter error: " << e.what() << std::endl;
                }
            }
        }
    }

    return nullptr;
}
