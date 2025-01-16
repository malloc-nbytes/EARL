#include <iostream>
#include <cassert>
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <optional>

#include "hidden-file.hpp"
#include "common.hpp"

std::vector<std::string> read_earl_file(void) {
    std::string home_dir = std::getenv("HOME");
    std::string file_path = home_dir + "/.earl";

    std::ifstream file(file_path);
    if (file) {
        std::ostringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();

        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line))
            lines.push_back(line);
        return lines;
    }
    else {
        return {};
    }
}

void parse_lines(std::vector<std::string> &lines) {
    auto tf_check = [&](auto &ans, uint32_t flag) {
        if (ans == "true")
            config::runtime::flags |= flag;
        else if (ans == "false")
            config::runtime::flags &= ~flag;
        else
            std::cerr << "invalid value: `" << ans << "`, expecting true or false" << std::endl;
    };

    auto get_comma_sep_values = [&](auto &ans) -> std::optional<std::vector<std::string>> {
        if (ans == "," || ans.empty())
            return std::nullopt;

        std::vector<std::string> values;
        std::istringstream ss(ans);
        std::string token;
        while (std::getline(ss, token, ',')) {
            values.push_back(token);
        }

        return values;
    };

    auto get_name = [&](auto &ans) {
        std::string name = ans;
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        return name;
    };

    auto handle_str = [&](auto &ans) {
        ans.erase(0, ans.find_first_not_of(" \t"));
        ans.erase(ans.find_last_not_of(" \t") + 1);
        if (ans.empty())
            return;
        config::repl::welcome::msg = ans;
    };

    for (const auto &line : lines) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        std::size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (key == COMMON_EARL2ARG_WITHOUT_STDLIB)
                tf_check(value, __WITHOUT_STDLIB);
            else if (key == COMMON_EARL2ARG_REPL_NOCOLOR)
                tf_check(value, __REPL_NOCOLOR);
            else if (key == COMMON_EARL2ARG_WATCH) {
                auto values = get_comma_sep_values(value);
                if (values.has_value()) {
                    for (const auto &v : values.value())
                        config::prelude::watch::files.push_back(get_name(v));
                    config::runtime::flags |= __WATCH;
                }
            }
            else if (key == COMMON_EARL2ARG_SHOWFUNS)
                tf_check(value, __SHOWFUNS);
            else if (key == COMMON_EARL2ARG_CHECK)
                tf_check(value, __CHECK);
            else if (key == COMMON_EARL2ARG_VERBOSE)
                tf_check(value, __VERBOSE);
            else if (key == COMMON_EARL2ARG_SHOWBASH)
                tf_check(value, __SHOWBASH);
            else if (key == COMMON_EARL2ARG_ERROR_ON_BASH_FAIL)
                tf_check(value, __ERROR_ON_BASH_FAIL);
            else if (key == COMMON_EARL2ARG_SHOWLETS)
                tf_check(value, __SHOWLETS);
            else if (key == COMMON_EARL2ARG_SHOWMUTS)
                tf_check(value, __SHOWMUTS);
            else if (key == COMMON_EARL2ARG_NO_SANITIZE_PIPES)
                tf_check(value, __NO_SANITIZE_PIPES);
            else if (key == COMMON_EARL2ARG_SUPPRESS_WARNINGS)
                tf_check(value, __SUPPRESS_WARNINGS);
            else if (key == COMMON_EARL2ARG_INCLUDE) {
                auto values = get_comma_sep_values(value);
                if (values.has_value()) {
                    for (const auto &v : values.value())
                        config::prelude::include::dirs.push_back(get_name(v));
                }
            }
            else if (key == COMMON_EARL2ARG_IMPORT) {
                auto values = get_comma_sep_values(value);
                if (values.has_value()) {
                    for (const auto &v : values.value())
                        config::prelude::import::dirs.push_back(get_name(v));
                }
            }
            else if (key == COMMON_EARL2ARG_REPL_THEME)
                config::repl::theme::selected = value;
            else if (key == COMMON_EARL2ARG_DISABLE_IMPLICIT_RETURNS)
                tf_check(value, __DISABLE_IMPLICIT_RETURNS);
            else if (key == COMMON_EARL2ARG_REPL_WELCOME)
                handle_str(value);
            else {
                std::cerr << "unrecognised key: `" << key << "`, failing config..." << std::endl;
                return;
            }
        }
        else {
            std::cerr << "No '=' found in line: '" << line << "', failing config..." << std::endl;
            return;
        }
    }
}

void create_default_config_file(void) {
    std::string home_dir = std::getenv("HOME");
    std::string file_path = home_dir + "/.earl";

    std::ofstream new_file(file_path);
    if (new_file) {
        std::string s = "";
        s += "# EARL Default Config File\n";
        s += "# NOTE: Follow this config *exactly* as the parser is not human friendly!\n";
        s += "#       Flags supplied through the CLI take priority over this config.\n\n";

        s += COMMON_EARL2ARG_WITHOUT_STDLIB "=false\n";
        s += COMMON_EARL2ARG_REPL_NOCOLOR "=false\n";
        s += COMMON_EARL2ARG_SHOWFUNS "=false\n";
        s += COMMON_EARL2ARG_CHECK "=false\n";
        s += COMMON_EARL2ARG_SUPPRESS_WARNINGS "=false\n";
        s += COMMON_EARL2ARG_VERBOSE "=false\n";
        s += COMMON_EARL2ARG_SHOWBASH "=false\n";
        s += COMMON_EARL2ARG_ERROR_ON_BASH_FAIL "=false\n";
        s += COMMON_EARL2ARG_SHOWLETS "=false\n";
        s += COMMON_EARL2ARG_SHOWMUTS "=false\n";
        s += COMMON_EARL2ARG_NO_SANITIZE_PIPES "=false\n";
        s += COMMON_EARL2ARG_DISABLE_IMPLICIT_RETURNS "=false\n\n";

        s += "# Takes a string without quotes. i.e., repl-welcome=this is a test message\n";
        s += COMMON_EARL2ARG_REPL_WELCOME "=""\n\n";

        s += "# Takes a comma separated list of values\n";
        s += COMMON_EARL2ARG_WATCH "=,\n";
        s += COMMON_EARL2ARG_IMPORT "=,\n";
        s += COMMON_EARL2ARG_INCLUDE "=,\n\n";

        s += "# Takes an identifier of the theme to use for the REPL\n";
        s += COMMON_EARL2ARG_REPL_THEME "=default";

        new_file << s;
        new_file.close();
    }
    else {
        std::cerr << "Failed to create .earl config file." << std::endl;
        std::exit(1);
    }

    std::cout << "Created a suspiciously default config file at: " << file_path << std::endl;
    std::exit(0);
}

void handle_hidden_file(void) {
    auto lines = read_earl_file();
    if (lines.size() != 0)
        parse_lines(lines);
}
