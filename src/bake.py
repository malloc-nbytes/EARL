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

# This file is used for baking the StdLib into the EARL
# executable making it the portable version. It creates
# a file called include/bake.hpp with the entirety of the
# StdLib.

import os
import sys

def embed_files(input_dir, output_file):
    file_map = {}

    for root, _, files in os.walk(input_dir):
        for file in files:
            filepath = os.path.join(root, file)
            print(f'baking: {filepath}')
            var_name = "std_" + os.path.relpath(filepath, input_dir).replace(os.sep, "_").replace(".", "_")
            with open(filepath, "r") as f:
                content = f.read()

            file_map[var_name] = content

    with open(output_file, "w") as out:
        out.write("#pragma once\n\n")
        out.write("// Embedded StdLib files\n\n")
        out.write("#include <map>\n\n")
        out.write("std::map<std::string, const char *> baked_stdlib = {\n")

        for var_name, content in file_map.items():
            out.write(f'    {{"{var_name}", R"__DELIM__({content})__DELIM__"}},\n')

        out.write("};\n")

if __name__ == "__main__":
    embed_files('src/std/', 'src/include/bake.hpp')
