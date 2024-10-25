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
#include <algorithm>
#include <functional>

#include "common.hpp"
#include "repled.hpp"
#include "utils.hpp"
#include "intrinsics.hpp"

static std::vector<std::string> KEYWORDS = COMMON_EARLKW_ASCPL;
static std::vector<std::string> TYPES = COMMON_EARLTY_ASCPL;

static std::vector<std::string> autocomplete = {};

static repled::PrefixTrie trie;

repled::PrefixTrie::PrefixTrie() : prefix(""), children(0), is_end_of_word(false) {}

repled::PrefixTrie::~PrefixTrie() {
    for (auto &child : children)
        delete child;
}

void repled::PrefixTrie::dump(void) const {
    std::vector<std::string> words;

    std::function<void(const PrefixTrie*, std::string)> collect_words = [&](const PrefixTrie *node, std::string current_word) {
        current_word += node->prefix;

        if (node->is_end_of_word)
            words.push_back(current_word);

        for (auto &child : node->children)
            collect_words(child, current_word);
    };

    collect_words(this, "");

    for (const auto &word : words)
        std::cout << word << ' ';

    std::cout << std::endl;
}

void repled::PrefixTrie::insert(std::string word) {
    PrefixTrie *current = this;
    for (auto &ch : word) {
        bool found = false;
        for (auto &child : current->children) {
            if (child->prefix == std::string(1, ch)) {
                current = child;
                found = true;
                break;
            }
        }
        if (!found) {
            PrefixTrie *new_node = new PrefixTrie();
            new_node->prefix = std::string(1, ch);
            current->children.push_back(new_node);
            current = new_node;
        }
    }
    current->is_end_of_word = true;
}

std::vector<std::string> repled::PrefixTrie::get_completions(std::string prefix) {
    std::vector<std::string> completions;
    PrefixTrie *current = this;

    for (auto &ch : prefix) {
        bool found = false;
        for (auto &child : current->children) {
            if (child->prefix == std::string(1, ch)) {
                current = child;
                found = true;
                break;
            }
        }
        if (!found)
            return completions; // No completions found
    }

    std::function<void(PrefixTrie*, std::string, bool)> collect_words = [&](PrefixTrie *node, std::string current_word, bool first) {
        if (!first)
            current_word += node->prefix;
        if (node->is_end_of_word) {
            completions.push_back(current_word);
        }
        for (auto &child : node->children) {
            collect_words(child, current_word, false);
        }
    };

    collect_words(current, prefix, true);
    return completions;
}

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

static std::string
get_last_word(std::string &line) {
    std::string buf = "";
    for (auto it = line.rbegin(); it != line.rend(); ++it) {
        if (!isalpha(*it))
            break;
        buf.insert(buf.begin(), *it);
    }
    return buf;
}

static void
redraw_line(std::string &line, std::string &prompt, int pad, repled::SS &ss, bool newline = false) {
    std::string yellow = "\033[93m";
    std::string blue = "\033[94m";
    std::string green = "\033[32m";
    std::string gray = "\033[90m";
    std::string red = "\033[31m";
    std::string underline = "\033[4m";
    std::string bold = "\033[1m";
    std::string italic = "\033[3m";
    std::string dim = "\033[2m";
    std::string invert = "\033[7m";
    std::string noc = "\033[0m";

    int local_braces = 0,
        local_brackets = 0,
        local_parens = 0;
    bool in_quote = false;
    bool comment = false;

    repled::clearln(line.size() + pad + prompt.size() + 50);

    std::cout << prompt;

    size_t i = 0;
    std::string current_word;
    while (i < line.size()) {
        if (line[i] == '#') comment = true;
        else if (line[i] == '}') ++local_braces;
        else if (line[i] == '{') --local_braces;
        else if (line[i] == ']') ++local_brackets;
        else if (line[i] == '[') --local_brackets;
        else if (line[i] == ')') ++local_parens;
        else if (line[i] == '(') --local_parens;

        // Handle a word boundary (space, quote, or single quote)
        if (isspace(line[i]) || line[i] == '"' || line[i] == '\'') {
            // If a word has been built, check if it's a keyword or type
            if (!current_word.empty()) {
                if (is_keyword(current_word)) {
                    if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                        std::cout << yellow << italic << bold << current_word;
                    else
                        std::cout << current_word;
                }
                else if (is_type(current_word)) {
                    if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                        std::cout << blue << current_word;
                    else
                        std::cout << current_word;
                }
                else {
                    if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                        std::cout << noc << current_word;
                    else
                        std::cout << current_word;
                }
                current_word.clear();
            }

            // Handle string literals (double quotes)
            if (line[i] == '"') {
                in_quote = true;
                if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                    std::cout << green << line[i];
                else
                    std::cout << line[i];
                size_t j = i + 1;
                // Print the content inside the string literal
                while (j < line.size() && line[j] != '"')
                    std::cout << line[j++];
                if (j < line.size() && line[j] == '"')
                    in_quote = false;
                if (j < line.size())
                    std::cout << line[j];
                if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                    std::cout << noc;
                i = j + 1;
            }
            // Handle character literals (single quotes)
            else if (line[i] == '\'') {
                if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                    std::cout << green << line[i];
                else
                    std::cout << line[i];
                size_t j = i + 1;
                // Print the content inside the character literal
                while (j < line.size() && line[j] != '\'')
                    std::cout << line[j++];
                if (j < line.size())
                    std::cout << line[j];
                if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                    std::cout << noc;
                i = j + 1;
            }
            else {
                // Print spaces or other non-alphabetic characters normally
                if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                    std::cout << noc << line[i];
                else
                    std::cout << line[i];
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
            if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                std::cout << yellow << current_word;
            else
                std::cout << current_word;
        }
        else if (is_type(current_word)) {
            if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                std::cout << blue << current_word;
            else
                std::cout << current_word;
        }
        else {
            if ((flags & __REPL_NOCOLOR) == 0 && !comment)
                std::cout << noc << current_word;
            else
                std::cout << current_word;
        }
    }

    // If the line does not end with a newline, handle the unmatched braces
    if (!newline && !in_quote && !comment) {
        size_t cursor_end_position = line.size() + pad;

        // Move the cursor to the end of the current line
        std::cout << "\033[" << (cursor_end_position + 6) << "G";

        // Move to the next line
        std::cout << "\033[1E";
        repled::clearln(50);

        std::string bracket_msg = "";
        bool bracket_ok = true;

        if (ss.braces - local_braces > 0) {
            bracket_msg += "{x" + std::to_string(ss.braces - local_braces) + ' ';
            bracket_ok = false;
        }
        else if (ss.braces - local_braces < 0) {
            bracket_msg += "}x" + std::to_string(local_braces - ss.braces) + ' ';
            bracket_ok = false;
        }

        if (ss.brackets - local_brackets > 0) {
            bracket_msg += "[x" + std::to_string(ss.brackets - local_brackets) + " ";
            bracket_ok = false;
        }
        else if (ss.brackets - local_brackets < 0) {
            bracket_msg += "]x" + std::to_string(local_brackets - ss.brackets) + " ";
            bracket_ok = false;
        }

        if (ss.parens - local_parens > 0) {
            bracket_msg += "(x" + std::to_string(ss.parens - local_parens) + " ";
            bracket_ok = false;
        }
        else if (ss.parens - local_parens < 0) {
            bracket_msg += ")x" + std::to_string(local_parens - ss.parens) + " ";
            bracket_ok = false;
        }

        if (bracket_ok) {
            if ((flags & __REPL_NOCOLOR) == 0)
                std::cout << noc << bold << dim << green << "< ok >" << noc << " ";
            else
                std::cout << "< ok > ";
        }
        else {
            if ((flags & __REPL_NOCOLOR) == 0)
                std::cout << noc << dim << italic << "< " << bracket_msg << ">" << noc << " ";
            else
                std::cout << "< " << bracket_msg << "> ";
        }

        // Extract the last word
        std::string last_word = get_last_word(line);
        if (!line.empty() && line[0] == ':') {
            last_word = ':' + last_word; // Prepend ':' if the line starts with it
        }

        std::vector<std::string> completions = trie.get_completions(last_word);

        if ((flags & __REPL_NOCOLOR) == 0)
            std::cout << gray << "| ";
        else
            std::cout << "| ";

        for (size_t i = 0; i < completions.size() && i < 7; ++i) {
            if (completions[i] == last_word) {
                if ((flags & __REPL_NOCOLOR) == 0)
                    std::cout << yellow << underline << completions[i] << noc << gray << " | ";
                else
                    std::cout << completions[i] << " | ";
            }
            else {
                if ((flags & __REPL_NOCOLOR) == 0)
                    std::cout << gray << completions[i] << " | ";
                else
                    std::cout << completions[i] << " | ";
            }
        }

        // Move the cursor back to the original line
        std::cout << "\033[A";

        // Move the cursor to the original position after printing
        std::cout << "\033[" << cursor_end_position + 2 << "G";
    }

    std::cout << noc << std::flush;
}

void
repled::clearln(int sz, bool flush) {
    std::cout << "\033[K" << "\r";
    // std::cout << "\r" << std::string(sz+8, ' ') << "\r";
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

    clearln(0);
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
        clearln(0);
        clearln(0, true);
        std::cout << prompt << std::flush;
        lines_idx = lines.size();
        line = "";
        return;
    }

    clearln(0);
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
    std::cout << "\033[K";

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
                    //c = line.size();
                    //std::cout << "\033[" << PAD+c+1 << "G";
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
        else if (ch == 0x0C) { // ctrl+l
            handle_clear_screen(prompt, c, PAD, line, ss);
            handle_jump_to_beginning_line(c, PAD, line, history);
        }
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

void
repled::init(std::vector<std::string> cmd_options) {
    std::vector<std::string> attrs = COMMON_EARLATTR_ASCPL;
    for (size_t i = 0; i < KEYWORDS.size(); ++i)
        trie.insert(KEYWORDS[i]);
    for (size_t i = 0; i < TYPES.size(); ++i)
        trie.insert(TYPES[i]);
    for (size_t i = 0; i < attrs.size(); ++i)
        trie.insert(attrs[i]);
    for (size_t i = 0; i < cmd_options.size(); ++i)
        trie.insert(cmd_options[i]);
    for (auto it = Intrinsics::intrinsic_functions.begin(); it != Intrinsics::intrinsic_functions.end(); ++it)
        trie.insert(it->first);
}

void repled::show_prefix_trie(void) {
    trie.dump();
}

