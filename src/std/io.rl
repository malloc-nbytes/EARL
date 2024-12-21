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

module IO

import "std/system.rl";

### Enum
#-- Name: Fd
#-- Parameter: In: int
#-- Parameter: Out: int
#-- Parameter: Err: int
#-- Description:
#--   The integer codes for `stdin`, `stdout`, and `stderr` for
#--   `In`, `Out`, and `Err` respectively.
@pub enum Fd {
    In = 0,
    Out,
    Err,
}
### End

### Function
#-- Name: file_to_str
#-- Parameter: fp: str
#-- Returns: str
#-- Description:
#--   Get the contents of a file `fp` and put it into a string.
@pub fn file_to_str(fp) {
    let f = open(fp, "r");
    let s = f.read();
    f.close();
    return s;
}
### End

### Function
#-- Name: read_lines
#-- Parameter: fp: str
#-- Returns: list
#-- Description:
#--   Read the lines of the file `fp` and get
#--   each line as a list.
@pub fn read_lines(fp: str): list {
    let content = file_to_str(fp);
    return content.split("\n");
}
### End

### Function
#-- Name: write_to_file
#-- Parameter: content: str
#-- Parameter: fp: str
#-- Returns: unit
#-- Description:
#--   Takes the string `content` and writes it to
#--   the file `fp`.
@pub fn write_to_file(content: str, fp: str): unit {
    let f = open(fp, "w");
    f.write(content);
    f.close();
}
### End

### Function
#-- Name: rename
#-- Parameter: path_from: str
#-- Parameter: path_to: str
#-- Returns: unit
#-- Description:
#--   Rename the file `path_from` to `path_to`.
@pub fn rename(path_from: str, path_to: str): unit {
    __internal_move__(path_from, path_to);
}
### End

fn __walkdir(@const @ref dir, @ref files) {
    foreach f in System::ls(dir) {
        if System::isdir(f) {
            __walkdir(f, files);
        }
        else {
            files.append(f);
        }
    }
}

### Function
#-- Name: walkdir
#-- Parameter: dir: str
#-- Returns: list<str>
#-- Description:
#--   Walks the directory `dir` recursively, returning
#--   all files found.
@pub fn walkdir(dir) {
    let files = [];
    __walkdir(dir, files);
    return files;
}
