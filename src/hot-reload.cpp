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

#include <cassert>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>

#include "hot-reload.hpp"

static std::unordered_map<std::filesystem::path, std::chrono::system_clock::time_point> last_writes;

static bool
is_newer(const std::filesystem::path &path,
         const std::chrono::system_clock::time_point &last_time) {
    try {
        if (!std::filesystem::exists(path)) {
            std::cerr << "File " << path << " does not exist\n";
            std::exit(1);
        }

        std::filesystem::file_time_type ft = std::filesystem::last_write_time(path);
        auto ft_sysclock =
            std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ft-std::filesystem::file_time_type::clock::now()+std::chrono::system_clock::now()
            );

        return ft_sysclock > last_time;

    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        std::exit(1);
    }

    return false; // unreachable
}

static void
update_last_write_time(const std::filesystem::path &path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::cerr << "File " << path << " does not exist\n";
            std::exit(1);
        }

        std::filesystem::file_time_type ft = std::filesystem::last_write_time(path);
        auto ft_sysclock = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ft-std::filesystem::file_time_type::clock::now()+std::chrono::system_clock::now()
        );

        last_writes[path] = ft_sysclock;

    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        std::exit(1);
    }
}

void
hot_reload::register_watch_files(std::vector<std::string> &watch_files) {
    assert(false);
}

bool
hot_reload::watch(void) {


    return true;
}

