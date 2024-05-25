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

#include <algorithm>
#include <iostream>
#include <vector>
#include <iostream>

#include "common.hpp"
#include "notify.hpp"
#include "utils.hpp"
#include "lexer.hpp"

std::vector<std::string> RESERVED = {"let", "def",};
std::vector<std::string> VARTYPES = {"int","str",};

const size_t VARTYPES_LEN = VARTYPES.size();

void usage(char *progname) {
  (void)progname;
  assert(false && "fixme");
}

std::vector<std::string> create_keywords(void) {
  std::vector<std::string> kws;
  std::for_each(RESERVED.begin(), RESERVED.end(), [&](std::string s) {kws.push_back(s);});
  std::for_each(VARTYPES.begin(), VARTYPES.end(), [&](std::string s) {kws.push_back(s);});
  return kws;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage(*argv);
  }

  char *filepath = *(++argv);

  std::vector<std::string> keywords = create_keywords();
  std::string comment = "#";

  Lexer lexer = lex_file(filepath, keywords, comment);
  lexer.dump();

  return 0;
}

