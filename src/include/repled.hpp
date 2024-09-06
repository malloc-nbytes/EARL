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
    struct RawInput {
        RawInput();
        ~RawInput();
        char get_char();
    private:
        struct termios old_termios;
    };

    void clearln(int sz, bool flush=false);
    void out_lineno(bool flush=false);
    void handle_backspace(std::string prompt, char ch, int &c, int pad, std::string &line, std::vector<std::string> &lines);
    void handle_newline(int &lines_idx, std::string &line, std::vector<std::string> &lines);
    void handle_up_arrow(std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines);
    void handle_down_arrow(std::string prompt, int &lines_idx, std::string &line, std::vector<std::string> &lines);
    void handle_left_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines);
    void handle_right_arrow(int &c, int pad, std::string &line, std::vector<std::string> &lines);
    void handle_tab(std::string prompt, int &c, int pad, std::string &line, std::vector<std::string> &lines);
    std::string getln(RawInput &RI, std::string prompt, std::vector<std::string> &history, bool bypass);
};

#endif // REPLED_H
