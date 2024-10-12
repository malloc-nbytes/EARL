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

repled::SS::SS() : braces(0), brackets(0), parens(0) {}

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
redraw_line(std::string &line, std::string &prompt, int pad, repled::SS &ss, bool newline = false) {
    std::string yellow = "\033[93m";
    std::string blue = "\033[94m";
    std::string green = "\033[32m";
    std::string gray = "\033[90m";
    std::string underline = "\033[4m";
    std::string bold = "\033[1m";
    std::string italic = "\033[3m";
    std::string dim = "\033[2m";
    std::string noc = "\033[0m";

    // Stacks to track unmatched symbols with their positions
    std::vector<std::pair<char, size_t>> opening_braces;
    std::vector<std::pair<char, size_t>> unmatched_closing;

    repled::clearln(line.size() + pad + prompt.size() + 50);
    std::cout << "\033[1E"; // Move to the next line
    repled::clearln(50);
    std::cout << "\033[A"; // Move back to the original line

    std::cout << prompt;

    size_t i = 0;
    std::string current_word;
    while (i < line.size()) {
        // Handle opening symbols by pushing to stack
        if (line[i] == '{' || line[i] == '(' || line[i] == '[')
            opening_braces.push_back({line[i], i});
        // Handle closing symbols by matching with stack
        else if (line[i] == '}' || line[i] == ')' || line[i] == ']') {
            if (!opening_braces.empty() &&
                ((line[i] == '}' && opening_braces.back().first == '{') ||
                 (line[i] == ')' && opening_braces.back().first == '(') ||
                 (line[i] == ']' && opening_braces.back().first == '['))) {
                // If the current closing symbol matches the last opening symbol, pop the stack
                opening_braces.pop_back();
            } else {
                // If no matching opening symbol, add this to unmatched closing
                unmatched_closing.push_back({line[i], i});
            }
        }

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
        if (is_keyword(current_word))
            std::cout << yellow << current_word;
        else if (is_type(current_word))
            std::cout << blue << current_word;
        else
            std::cout << noc << current_word;
    }

    // If the line does not end with a newline, handle the unmatched braces
    if (!newline) {
        size_t cursor_end_position = line.size() + pad;

        // Move the cursor to the end of the current line
        std::cout << "\033[" << (cursor_end_position + 6) << "G";

        // Print unmatched opening symbols first (in the correct order)
        std::string msg = "";
        for (const auto& pair : opening_braces)
            msg += pair.first;

        int local_braces = ss.braces;
        int local_brackets = ss.brackets;
        int local_parens = ss.parens;

        // Then print the unmatched closing symbols (in the order they were encountered)
        for (const auto& pair : unmatched_closing) {
            if (pair.first == '}' && local_braces > 0)
                --local_braces;
            else if (pair.first == ']' && local_brackets > 0)
               --local_brackets;
            else if (pair.first == ')' && local_parens > 0)
                --local_parens;
            else
                msg += pair.first;
        }

        if (local_braces > 0)
            msg += std::string(local_braces, '{');
        if (local_brackets > 0)
            msg += std::string(local_brackets, '[');
        if (local_parens > 0)
            msg += std::string(local_parens, '(');

        // Move to the next line
        std::cout << "\033[1E";
        repled::clearln(50);

        // Print the message
        if (!msg.empty())
            std::cout << underline << gray << "unclosed: " << msg << " in no order";
        else
            std::cout << bold << green << "ok";

        // Move the cursor back to the original line
        std::cout << "\033[A";

        // Move the cursor to the original position after printing
        std::cout << "\033[" << cursor_end_position + 2 << "G";
    }

    std::cout << noc << std::flush;
}

void
repled::clearln(int sz, bool flush) {
    std::cout << "\r" << std::string(sz+8, ' ') << "\r";
    if (flush)
        std::cout.flush();
}

void
repled::handle_backspace(std::string prompt, char ch, int &c, int pad, std::string &line, std::vector<std::string> &lines, repled::SS &ss) {
    if (c <= 0)
        return;

    std::cout << "\033[D" << std::flush;

    line.erase(c-1, 1);

    redraw_line(line, prompt, pad, ss);
    std::cout << "\033[" << c+pad << "G" << std::flush;
    --c;
}

static void
handle_delete(std::string &prompt, char ch, int &c, int pad, std::string &line, std::vector<std::string> &history, repled::SS &ss) {
    if (c >= static_cast<int>(line.size()))
        return;

    std::cout << "\033[P" << std::flush;

    line.erase(c, 1);

    redraw_line(line, prompt, pad, ss);
    std::cout << "\033[" << c+pad+1 << "G" << std::flush;
}

void
handle_delete_to_end(std::string &prompt, int &c, int pad, std::string &line, std::vector<std::string> &history, repled::SS &ss) {
    if (c >= static_cast<int>(line.size()))
        return;

    line.erase(c, line.size()-c);

    redraw_line(line, prompt, pad, ss);
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
repled::handle_up_arrow(int c, int pad, std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines, repled::SS &ss) {
    if (lines.size() == 0)
        return;
    if (lines_idx <= 0)
        return;

    --lines_idx;
    std::string &histline = lines[lines_idx];
    redraw_line(histline, prompt, line.size(), ss);
    line = histline;
    handle_jump_to_end(c, pad, line, lines);
}

void
repled::handle_down_arrow(int c, int pad, std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines, repled::SS &ss) {
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
    redraw_line(histline, prompt, line.size()+prompt.size(), ss);
    line = histline;
    handle_jump_to_end(c, pad, line, lines);
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
jump_word(int &c, int sz, std::string &line) {
    if (c >= sz)
        return;

    if (isalnum(line[c])) {
        while (c < sz && isalnum(line[c]))
            ++c;
        while (c < sz && !isalnum(line[c]))
            ++c;
    }
    else {
        while (c < sz && !isalnum(line[c]))
            ++c;
        while (c < sz && isalnum(line[c]))
            ++c;
    }
    std::cout << "\033[" << c+1 << "G" << std::flush;
}

void
jump_word_rev(int &c, int sz, std::string &line) {
    if (c <= 0)
        return;

    if (isalnum(line[c])) {
        while (c > 0 && isalnum(line[c]))
            --c;
        while (c < sz && !isalnum(line[c]))
            --c;
    }
    else {
        while (c > 0 && !isalnum(line[c-1]))
            --c;
        while (c > 0 && isalnum(line[c-1]))
            --c;
    }
    std::cout << "\033[" << c+1 << "G" << std::flush;
}

void
delete_word_from_cursor(std::string &prompt, int &c, int pad, std::string &line, repled::SS &ss) {
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
    redraw_line(line, prompt, pad, ss);
    std::cout << "\033[" << c+pad+1 << "G" << std::flush;
}

void
handle_clear_screen(std::string &prompt, int &c, int pad, std::string &line, repled::SS &ss) {
    if (system("clear") == 0) {
        std::cout << prompt << line;
        std::cout << "\033[" << c+pad+1 << "G" << std::flush;
    }
    redraw_line(line, prompt, 0, ss);
}

std::string
repled::getln(RawInput &RI, std::string prompt, std::vector<std::string> &history, bool bypass, repled::SS &ss) {
    const int PAD = prompt.size();
    std::string line = "";
    int lines_idx = history.size();
    int c = 0;

    bool ready = prompt[0] != '0' && !bypass;

    if (ready) {
        std::cout << prompt;

        // Move cursor to line below.
        std::cout << "\033[1E";
        repled::clearln(50);

        std::cout << "[";
        if ((flags & __REPL_NOCOLOR) == 0)
            std::cout << "\033[32m";
        std::cout << "Enter to Evaluate";
        if ((flags & __REPL_NOCOLOR) == 0)
            std::cout << "\033[0m";

        std::cout << "]" << "\033[" << PAD+1 << "G" << std::flush;
        // Move cursor to prev line.
        std::cout << "\033[A";

        handle_jump_to_beginning_line(c, PAD, line, history);
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

        if (ENTER(ch)) {
            redraw_line(line, prompt, PAD-1, ss, /*newline=*/true);
            break;
        }

        else if (TAB(ch))
            handle_tab(prompt, c, PAD, line, history);

        else if (ESCSEQ(ch)) {
            int next0 = RI.get_char();
            if (CSI(next0)) {
                int next1 = RI.get_char();
                switch (next1) {
                case UP_ARROW: {
                    handle_up_arrow(c, PAD, prompt, lines_idx, line, history, ss);
                    c = line.size();
                    std::cout << "\033[" << PAD+c+1 << "G";
                } break;
                case DOWN_ARROW: {
                    handle_down_arrow(c, PAD, prompt, lines_idx, line, history, ss);
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
                    delete_word_from_cursor(prompt, c, PAD, line, ss);
                } break;
                default: break;
                }
            }
        }
        else if (ch == 0x0C)
            handle_clear_screen(prompt, c, PAD, line, ss);
        else if (ch == 0x01) // ctrl+a
            handle_jump_to_beginning_line(c, PAD, line, history);
        else if (ch == 0x02) // ctrl+b
            handle_left_arrow(c, PAD, line, history);
        else if (ch == 0x06) // ctrl+f
            handle_right_arrow(c, PAD, line, history);
        else if (ch == 0x04) // ctrl+d
            handle_delete(prompt, ch, c, PAD, line, history, ss);
        else if (ch == 0x0B) // ctrl+k
            handle_delete_to_end(prompt, c, PAD, line, history, ss);
        else if (ch == 0x0E) { // ctrl+n
            handle_down_arrow(c, PAD, prompt, lines_idx, line, history, ss);
            c = line.size();
            std::cout << "\033[" << PAD+c+1 << "G";
        }
        else if (ch == 0x10) { // ctrl+p
            handle_up_arrow(c, PAD, prompt, lines_idx, line, history, ss);
            c = line.size();
            std::cout << "\033[" << PAD+c+1 << "G";
        }
        else if (ch == 0x05) // ctrl+e
            handle_jump_to_end(c, PAD, line, history);
        else if (BACKSPACE(ch))
            handle_backspace(prompt, ch, c, PAD, line, history, ss);
        else {
            if (c != line.size()) {
                line.insert(line.begin()+c, ch);
                redraw_line(line, prompt, PAD-1, ss);
                std::cout << "\033[" << c+PAD+2 << "G";
            }
            else {
                line.push_back(ch);
                redraw_line(line, prompt, PAD-1, ss);
            }
            ++c;
            std::cout.flush();
        }
    }

    return line;
}
