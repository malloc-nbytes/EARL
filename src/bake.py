import os
import sys

def embed_files(input_dir, output_file):
    with open(output_file, "w") as out:
        out.write("#pragma once\n\n")
        out.write("// Embedded stdlib files\n\n")
        for root, _, files in os.walk(input_dir):
            for file in files:
                filepath = os.path.join(root, file)
                print(f'baking: {filepath}')
                var_name = os.path.relpath(filepath, input_dir).replace(os.sep, "_").replace(".", "_")
                with open(filepath, "r") as f:
                    content = f.read().encode("unicode_escape").decode("ascii")
                out.write(f"const char* {var_name} = R\"({content})\";\n")

if __name__ == "__main__":
    embed_files('std/', 'include/bake.hpp')
