# MIT License

# Copyright (c) 2023 malloc-nbytes

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

### IGNORE

module TOML

import "std/datatypes/str.rl";
import "std/io.rl";
import "std/assert.rl";

Assert::FILE = __FILE__;

fn parse_section_name(line) {
    let id = line.substr(1, len(line)-2);
    return id;
}

fn parse_assignment(line) {
    Assert::FUNC = __FUNC__;

    let parts = line.split("=");
    Assert::eq(len(parts), 2);
    let id, value = (parts[0], parts[1]);
    Str::trim(id);
    Str::trim(value);
    return (id, value);
}

fn parse_lines(lines: list): dictionary {
    let d = Dict(str);
    let i = 0;
    let section = none;
    while i < len(lines) {
        Assert::FUNC = __FUNC__;

        let line = lines[i];

        if line[0] == '#' {
            i += 1;
            continue;
        }
        else if line[0] == '[' {
            section = some(parse_section_name(line));
            d.insert(section.unwrap(), Dict(str));
        }
        else {
            let var, value = parse_assignment(line);
            Assert::not_none(section);
            d[section.unwrap()].unwrap().insert(var, value);
        }

        i += 1;
    }
    return d;
}

@pub @experimental
fn parse(fp: str): dictionary {
    let lines = IO::file_to_str(fp)
                    .split("\n")
                    .filter(|s| {
                        return s != "";
                    });

    return parse_lines(lines);
}

@pub fn dump(d: dictionary): unit {
    fn spaces(s) {
        for i in 0 to s { print(' '); }
    }

    fn aux(value, s) {
        if type(value) == "DictStr" {
            foreach k, v in value {
                spaces(s);
                print(f"{k} = ");
                aux(v, s+2);
            }
        }
        else {
            println(value);
        }
    }

    foreach k, v in d {
        println(f"[{k}]:");
        aux(v, 2);
    }
}

