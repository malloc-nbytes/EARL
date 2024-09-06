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

#include "repled.hpp"

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

    clearln(line.size()+pad);
    std::cout << prompt << line;
    // std::cout << "\033[" << (line.size() - (c-1)) << "D";
    std::cout << "\033[" << c+pad << "G" << std::flush;
    --c;
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
    clearln(line.size());
    std::cout << prompt << histline << std::flush;
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
    clearln(line.size());
    std::cout << prompt << histline;
    line = histline;
    std::cout.flush();
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
        std::cout << "\033[32m";
        std::cout << "ENTER TO EVAL";
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
        }
        else if (BACKSPACE(ch)) {
            handle_backspace(prompt, ch, c, PAD, line, history);
        }
        else {
            if (c != line.size()) {
                clearln(line.size());
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
