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

#include <thread>
#include <cassert>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>

#include "hot-reload.hpp"

#ifdef __APPLE__
struct PathHash {
    std::size_t operator()(const std::filesystem::path& path) const {
        return std::hash<std::string>()(path.string());
    }
};
struct PathEqual {
    bool operator()(const std::filesystem::path& lhs,
                    const std::filesystem::path& rhs) const {
        return lhs == rhs;
    }
};
static std::unordered_map<std::filesystem::path,
                          std::filesystem::file_time_type,
                          PathHash,
                          PathEqual> last_writes;
#else
static std::unordered_map<std::filesystem::path,
                          std::filesystem::file_time_type> last_writes;
#endif

static bool
is_newer(const std::filesystem::path &path,
         const std::filesystem::file_time_type &last_time) {
    try {
        if (!std::filesystem::exists(path)) {
            std::cerr << "File " << path << " does not exist\n";
            return false;
        }

        auto ft = std::filesystem::last_write_time(path);
        return ft > last_time;

    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        std::exit(1);
    }
}

static void
update_last_write_time(const std::filesystem::path &path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::cerr << "File " << path << " does not exist\n";
            return;
        }

        auto ft = std::filesystem::last_write_time(path);
        last_writes[path] = ft;

    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        std::exit(1);
    }
}

void
hot_reload::register_watch_files(std::vector<std::string> &watch_files) {
    for (const auto &f : watch_files) {
        std::filesystem::path file_path(f);
        if (std::filesystem::exists(file_path)) {
            last_writes[file_path] = std::filesystem::last_write_time(file_path);
        } else {
            std::cerr << "File " << file_path << " does not exist at registration\n";
        }
    }
}

void
hot_reload::watch(void) {
    while (true) {
        bool file_changed = false;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        for (auto it = last_writes.begin(); it != last_writes.end(); ++it) {
            if (is_newer(it->first, it->second)) {
                update_last_write_time(it->first);
                file_changed = true;
            }
        }

        if (file_changed) {
            // Exit loop if any file changed
            return;
        }
    }
}
