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

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "err.hpp"
#include "common.hpp"

int
levenshtein_distance(std::string &str1, std::string &str2) {
    const size_t m = str1.length();
    const size_t n = str2.length();

    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));

    for (size_t i = 0; i <= m; ++i)
        dp[i][0] = i;

    for (size_t j = 0; j <= n; ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            if (str1[i-1] == str2[j-1])
                dp[i][j] = dp[i-1][j-1];
            else {
                dp[i][j] = 1 + std::min(dp[i][j-1], std::min(dp[i-1][j], dp[i-1][j-1]));
            }
        }
    }

    return dp[m][n];
}

bool
valid_earl_cli_flag(const std::string &flag) {
    std::vector<std::string> flags2 = COMMON_EARL2ARG_ASCPL;
    std::vector<char> flags1 = COMMON_EARL1ARG_ASCPL;

    if (flag.size() <= 2)
        return false;
    if (flag.at(0) != '-')
        return false;

    if (flag.at(0) == '-' && flag.at(0) == '-') {
        std::string trim = flag.substr(2);
        for (auto &f : flags2)
            if (trim == f)
                return true;
        return false;
    }

    return false;
}

std::string
file_to_cxxstring(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file)
        throw InterpreterException("could not open file: " + filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string>
file_lines_to_vec(const std::string &path) {
    std::vector<std::string> res = {};
    std::string content = file_to_cxxstring(path);

    std::string buf = {};
    for (size_t i = 0; i < content.size(); ++i) {
        char c = content[i];
        if (c == '\n') {
            res.push_back(buf);
            buf.clear();
        } else {
            buf.push_back(c);
        }
    }

    res.push_back(buf);

    return res;
}

