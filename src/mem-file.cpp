#include <cassert>

#include "mem-file.hpp"
#include "utils.hpp"
#include "err.hpp"

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
get_mem_file(void) {
    return std::string(get_home()) + "/.earl_mem";
}

std::unordered_map<std::string, std::string>
parse_mem_file(void) {
    assert(0);
}
