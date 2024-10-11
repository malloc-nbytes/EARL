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

#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "common.hpp"
#include "repled.hpp"

static std::vector<std::string> KEYWORDS = COMMON_EARLKW_ASCPL;
static std::vector<std::string> TYPES = COMMON_EARLTY_ASCPL;

repled::RawInput::RawInput() {
    tcgetattr(STDIN_FILENO, &old_termios);
    termios raw = old_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

repled::RawInput::~RawInput() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

char
repled::RawInput::get_char() {
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
}

static bool
is_keyword(std::string &word) {
    for (auto &kw : KEYWORDS)
        if (word == kw)
            return true;
    return false;
}

static bool
is_type(std::string &word) {
    for (auto &ty : TYPES)
        if (word == ty)
            return true;
    return false;
}

static void
redraw_line(std::string &line, std::string &prompt, int pad) {
    std::string yellow = "\033[93m";
    std::string blue = "\033[94m";
    std::string green = "\033[32m";
    std::string gray = "\033[90m";
    std::string underline = "\033[4m";
    std::string bold = "\033[1m";
    std::string italic = "\033[3m";
    std::string noc = "\033[0m";

    repled::clearln(line.size() + pad + prompt.size() + 50);
    std::cout << prompt;

    size_t i = 0;
    std::string current_word;
    while (i < line.size()) {
        // Handle a word boundary (space, quote, or single quote)
        if (isspace(line[i]) || line[i] == '"' || line[i] == '\'') {
            // If a word has been built, check if it's a keyword or type
            if (!current_word.empty()) {
                if (is_keyword(current_word))
                    std::cout << yellow << italic << bold << current_word;
                else if (is_type(current_word))
                    std::cout << blue << current_word;
                else
                    std::cout << noc << current_word;
                current_word.clear();
            }

            // Handle string literals (double quotes)
            if (line[i] == '"') {
                std::cout << green << line[i];
                size_t j = i + 1;
                // Print the content inside the string literal
                while (j < line.size() && line[j] != '"') {
                    std::cout << line[j++];
                }
                if (j < line.size()) {
                    std::cout << line[j];
                }
                std::cout << noc;
                i = j + 1;
            }
            // Handle character literals (single quotes)
            else if (line[i] == '\'') {
                std::cout << green << line[i];
                size_t j = i + 1;
                // Print the content inside the character literal
                while (j < line.size() && line[j] != '\'') {
                    std::cout << line[j++];
                }
                if (j < line.size()) {
                    std::cout << line[j];
                }
                std::cout << noc;
                i = j + 1;
            }
            else {
                // Print spaces or other non-alphabetic characters normally
                std::cout << noc << line[i];
                ++i;
            }
        } else {
            // Accumulate characters for a word (could be a keyword, type, etc.)
            current_word += line[i];
            ++i;
        }
    }

    // Handle the last word in case the line ends without a space
    if (!current_word.empty()) {
        if (is_keyword(current_word)) {
            std::cout << yellow << current_word;
        } else if (is_type(current_word)) {
            std::cout << blue << current_word;
        } else {
            std::cout << noc << current_word;
        }
    }

    // size_t cursor_end_position = line.size() + pad;
    // std::cout << "\033[" << (cursor_end_position + 6) << "G";

    // if (g_brace != 0) {
    //     const std::string msg = "missing "+std::to_string(g_brace)+"`}`";
    //     std::cout << gray << msg << "\033[" << msg.size() << "D" << std::flush;
    // }
    // else {
    //     std::cout << green << "ok";
    //     std::cout << "\033[6D" << std::flush;
    // }

    // if (line.back() == ';') {
    //     std::cout << green << "ok";
    //     std::cout << "\033[6D" << std::flush;
    // }
    // else {
    //     std::cout << gray << "x";
    //     std::cout << "\033[5D" << std::flush;
    // }

    std::cout << noc << std::flush;
}

void
repled::clearln(int sz, bool flush) {
    std::cout << "\r" << std::string(sz+8, ' ') << "\r";
    if (flush)
        std::cout.flush();
}

void
repled::handle_backspace(std::string prompt, char ch, int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    if (c <= 0)
        return;

    std::cout << "\033[D" << std::flush;

    line.erase(c-1, 1);

    // clearln(line.size()+pad);
    // std::cout << prompt << line;
    redraw_line(line, prompt, pad);
    std::cout << "\033[" << c+pad << "G" << std::flush;
    --c;
}

static void
handle_delete(std::string &prompt, char ch, int &c, int pad, std::string &line, std::vector<std::string> &history) {
    if (c >= static_cast<int>(line.size()))
        return;

    std::cout << "\033[P" << std::flush;

    line.erase(c, 1);

    redraw_line(line, prompt, pad);
    std::cout << "\033[" << c+pad+1 << "G" << std::flush;
}

void
handle_delete_to_end(std::string &prompt, int &c, int pad, std::string &line, std::vector<std::string> &history) {
    if (c >= static_cast<int>(line.size()))
        return;

    line.erase(c, line.size()-c);

    redraw_line(line, prompt, pad);
    std::cout << "\033[" << c+pad+1 << "G" << std::flush;
}

void
repled::handle_newline(int &lines_idx, std::string &line, std::vector<std::string> &lines) {
    std::cout << std::endl;
    if (line != "") {
        lines.push_back(line);
        line.clear();
        lines_idx = lines.size()-1;
    }
}

void
repled::handle_up_arrow(std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines) {
    if (lines.size() == 0)
        return;
    if (lines_idx <= 0)
        return;

    --lines_idx;
    std::string &histline = lines[lines_idx];
    redraw_line(histline, prompt, line.size());
    line = histline;
}

void
repled::handle_down_arrow(std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines) {
    if (lines.size() == 0)
        return;
    if (lines_idx >= static_cast<int>(lines.size()) - 1) {
        clearln(line.size());
        std::cout << prompt << std::flush;
        lines_idx = lines.size();
        line = "";
        return;
    }

    ++lines_idx;
    std::string &histline = lines[lines_idx];
    redraw_line(histline, prompt, line.size()+prompt.size());
    line = histline;
}

void
repled::handle_left_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    if (c <= 0)
        return;
    --c;
    std::cout << "\033[D" << std::flush;
}

void
repled::handle_right_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    if (c >= static_cast<int>(line.size()))
        return;
    ++c;
    std::cout << "\033[C" << std::flush;
}

void
repled::handle_tab(std::string prompt, int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    clearln(line.size());
    line.insert(line.begin()+c, ' ');
    line.insert(line.begin()+c, ' ');
    std::cout << prompt;
    std::cout << line;
    std::cout << "\033[" << c+pad+3 << "G" << std::flush;
    c += 2;
}

void
handle_jump_to_end(int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    c = line.size();
    std::cout << "\033[" << c+pad+1 << "G" << std::flush;
}

void
handle_jump_to_beginning_line(int &c, int pad, std::string &line, std::vector<std::string> &lines) {
    c = 0;
    std::cout << "\033[" << pad+1 << "G" << std::flush;
}

void
jump_word(int &c, int sz, std::string &line) {
    if (c >= sz)
        return;

    while (c < sz && !isspace(line[c]))
        ++c;
    while (c < sz && isspace(line[c]))
        ++c;
    std::cout << "\033[" << c+1 << "G" << std::flush;
}

void
jump_word_rev(int &c, int sz, std::string &line) {
    if (c <= 0)
        return;

    while (c > 0 && isspace(line[c-1]))
        --c;
    while (c > 0 && !isspace(line[c-1]))
        --c;
    std::cout << "\033[" << c+1 << "G" << std::flush;
}

void
delete_word_from_cursor(std::string &prompt, int &c, int pad, std::string &line, std::vector<std::string> &history) {
    if (c >= static_cast<int>(line.size()))
        return;

    if (!isalnum(line[c])) {
        int i = 0;
        while (c+i < static_cast<int>(line.size()) && !isalnum(line[c+i])) {
            ++i;
        }
        while (c+i < static_cast<int>(line.size()) && isalnum(line[c+i]))
            ++i;
        line.erase(c, i);
    }
    else {
        int i = c;
        while (i < static_cast<int>(line.size()) && isalnum(line[i]))
            ++i;
        line.erase(c, i-c);
    }
    redraw_line(line, prompt, pad);
    std::cout << "\033[" << c+pad+1 << "G" << std::flush;
}

void
handle_clear_screen(std::string &prompt, int &c, int pad, std::string &line) {
    if (system("clear") == 0) {
        std::cout << prompt << line;
        std::cout << "\033[" << c+pad+1 << "G" << std::flush;
    }
    redraw_line(line, prompt, 0);
}

std::string
repled::getln(RawInput &RI, std::string prompt, std::vector<std::string> &history, bool bypass) {
    const int PAD = prompt.size();
    std::string line = "";
    int lines_idx = history.size();
    int c = 0;

    bool ready = prompt[0] != '0' && !bypass;

    if (ready) {
        std::cout << prompt << std::string(64, ' ');
        std::cout << "[";
        if ((flags & __REPL_NOCOLOR) == 0)
            std::cout << "\033[32m";
        std::cout << "ENTER TO EVAL";
        if ((flags & __REPL_NOCOLOR) == 0)
            std::cout << "\033[0m";
        std::cout << "]" << "\033[" << PAD+1 << "G" << std::flush;
    }
    else
        std::cout << prompt << std::flush;

    while (1) {
        char ch = RI.get_char();

        if (ready) {
            repled::clearln(line.size()+76);
            std::cout << prompt << std::flush;
            ready = false;
        }

        if (ENTER(ch))
            break;

        else if (TAB(ch))
            handle_tab(prompt, c, PAD, line, history);

        else if (ESCSEQ(ch)) {
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
            else { // alt key
                switch (next0) {
                case 'f': {
                    jump_word(c, line.size(), line);
                    std::cout << "\033[" << c+PAD+1 << "G" << std::flush;
                } break;
                case 'b': {
                    jump_word_rev(c, line.size(), line);
                    std::cout << "\033[" << c+PAD+1 << "G" << std::flush;
                } break;
                case 'd': {
                    delete_word_from_cursor(prompt, c, PAD, line, history);
                } break;
                default: break;
                }
            }
        }
        else if (ch == 0x0C)
            handle_clear_screen(prompt, c, PAD, line);
        else if (ch == 0x01) // ctrl+a
            handle_jump_to_beginning_line(c, PAD, line, history);
        else if (ch == 0x02) // ctrl+b
            handle_left_arrow(c, PAD, line, history);
        else if (ch == 0x06) // ctrl+f
            handle_right_arrow(c, PAD, line, history);
        else if (ch == 0x04) // ctrl+d
            handle_delete(prompt, ch, c, PAD, line, history);
        else if (ch == 0x0B) // ctrl+k
            handle_delete_to_end(prompt, c, PAD, line, history);
        else if (ch == 0x0E) { // ctrl+n
            handle_down_arrow(prompt, lines_idx, line, history);
            c = line.size();
            std::cout << "\033[" << PAD+c+1 << "G";
        }
        else if (ch == 0x10) { // ctrl+p
            handle_up_arrow(prompt, lines_idx, line, history);
            c = line.size();
            std::cout << "\033[" << PAD+c+1 << "G";
        }
        else if (ch == 0x05) // ctrl+e
            handle_jump_to_end(c, PAD, line, history);
        else if (BACKSPACE(ch))
            handle_backspace(prompt, ch, c, PAD, line, history);
        else {
            if (c != line.size()) {
                line.insert(line.begin()+c, ch);
                redraw_line(line, prompt, PAD-1);
                std::cout << "\033[" << c+PAD+2 << "G";
            }
            else {
                line.push_back(ch);
                redraw_line(line, prompt, PAD-1);
            }
            ++c;
            std::cout.flush();
        }
    }

    return line;
}
