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

#ifndef REPLED_H
#define REPLED_H

#include <termios.h>

// FORGROUNDS
#define YELLOW "\033[93m"
#define GREEN "\033[32m"
#define BRIGHT_GREEN "\033[92m"
#define GRAY "\033[90m"
#define RED "\033[31m"
#define BLUE "\033[94m"
#define CYAN "\033[96m"
#define MAGENTA "\033[95m"
#define WHITE "\033[97m"
#define BLACK "\033[30m"
#define CYAN "\033[96m"
#define PINK "\033[95m"
#define BRIGHT_PINK "\033[38;5;213m"
#define PURPLE "\033[35m"
#define BRIGHT_PURPLE "\033[95m"
#define ORANGE "\033[38;5;214m"
#define BROWN "\033[38;5;94m"

// BACKGROUNDS
#define DARK_BLUE_BACKGROUND "\033[44m"

// EFFECTS
#define UNDERLINE "\033[4m";
#define BOLD "\033[1m";
#define ITALIC "\033[3m";
#define DIM "\033[2m";
#define INVERT "\033[7m";

// RESET
#define NOC "\033[0m"

#define ENTER(ch) (ch)=='\n'
#define BACKSPACE(ch) (ch) == 8 || (ch) == 127
#define ESCSEQ(ch) (ch) == 27
#define CSI(ch) (ch) == '['
#define TAB(ch) (ch) == '\t'
#define UP_ARROW 'A'
#define DOWN_ARROW 'B'
#define RIGHT_ARROW 'C'
#define LEFT_ARROW 'D'

namespace repled {
    const char *main_color(void);
    const char *keyword_color(void);
    const char *misc_color(void);
    const char *type_color(void);
    const char *quote_color(void);
    const char *error_color(void);
    const char *msg_color(void);
    const char *stdout_color(void);
    const char *stdout_type_color(void);
    const char *status_ok(void);

    struct RawInput {
        RawInput();
        ~RawInput();
        char get_char();
    private:
        struct termios old_termios;
    };

    struct SS {
        int braces;
        int brackets;
        int parens;
        SS();
        ~SS() = default;
    };

    struct PrefixTrie {
        std::string prefix;
        std::vector<PrefixTrie*> children;
        bool is_end_of_word;

        PrefixTrie();
        ~PrefixTrie();

        void insert(std::string word);
        std::vector<std::string> get_completions(std::string prefix);
        void dump(void) const;
    };

    void clearln(int sz, bool flush=false);
    void out_lineno(bool flush=false);
    void handle_backspace(std::string prompt, char ch, int &c, int pad, std::string &line, std::vector<std::string> &lines, repled::SS &ss);
    void handle_newline(int &lines_idx, std::string &line, std::vector<std::string> &lines);
    void handle_up_arrow(int c, int pad, std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines, repled::SS &ss);
    void handle_down_arrow(int c, int pad, std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines, repled::SS &ss);
    void handle_left_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines);
    void handle_right_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines);
    void handle_tab(std::string prompt, int &c, int pad, std::string &line, std::vector<std::string> &lines);
    std::string getln(RawInput &RI, std::string prompt, std::vector<std::string> &history, bool bypass, repled::SS &ss);
    void init(std::vector<std::string> cmd_options);
    void show_prefix_trie(void);
    void write_last_line(const std::string &content);
    int get_cursor_row();
    int get_terminal_height();
};

#endif // REPLED_H
