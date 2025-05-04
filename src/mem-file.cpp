#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include "mem-file.hpp"
#include "utils.hpp"
#include "err.hpp"
#include "common.hpp"

static char *
get_home(void) {
    char *home = getenv("HOME");
    if (!home) {
        const std::string msg = "could not get the value of $HOME";
        throw InterpreterException(msg);
    }
    return home;
}

static std::string
get_mem_filepath(void) {
    return std::string(get_home()) + "/.earl_mem";
}

void
init_mem_file(void) {
    std::string fp = get_mem_filepath();
    std::ifstream in(fp);
    if (in.is_open()) {
        in.close();
        return;
    }
    std::ofstream out(fp);
    if (out.is_open()) {
        out.close();
        return;
    }
    std::string msg = "could not open .earl_mem for persistent memory";
    throw InterpreterException(msg);
}

std::unordered_map<std::string, std::string>
parse_mem_file(void) {
    ////////////////////////////
    // --- Format ---
    // x=3
    // s=foo bar baz
    // c=a
    // b=true
    // f=1.2
    ////////////////////////////

    std::unordered_map<std::string, std::string> res;
    std::vector<std::string> lines = file_lines_to_vec(get_mem_filepath());

    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string &line = lines[i];
        if (line.size() == 0 || line == "\n") {
            continue;
        }

        std::string id = "", value = "";
        bool eq = false;
        for (size_t j = 0; j < line.size(); ++j) {
            char c = line[j];
            if (!eq && c == '=') {
                eq = true;
            }
            else if (!eq) {
                id += c;
            } else {
                value += c;
            }
        }

        res[id] = value;
    }

    return res;
}

void
write_mem_file(std::unordered_map<std::string, std::string> &content) {
    std::ofstream out(get_mem_filepath());
    if (!out.is_open()) {
        std::string msg = "could not open .earl_mem for persistent memory";
        throw InterpreterException(msg);
    }

    for (auto &entry : content) {
        out << entry.first << "=" << entry.second << "\n";
    }
}

void
clear_mem_file(void) {
    config::runtime::persistent_mem.clear();
    write_mem_file(config::runtime::persistent_mem);
}
