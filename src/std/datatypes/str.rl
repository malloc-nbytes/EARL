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

module Str

import "std/datatypes/char.rl";

### Function
#-- Name: isnum
#-- Parameter: s: str
#-- Returns: bool
#-- Description:
#--   Check to see if the string `s` is a number.
@pub fn isnum(s) {
    foreach c in s {
        if !Char::isnum(c) {
            return false;
        }
    }
    return true;
}
### End

### Function
#-- Name: to_list
#-- Parameter: s: @ref str
#-- Returns: list
#-- Description:
#--   Convert the string `s` to a `list`.
@pub fn to_list(@ref s) {
    let lst = [];
    for i in 0 to len(s) {
        lst.append(s[i]);
    }
    return lst;
}
### End

### Function
#-- Name: find
#-- Parameter: s: @ref str
#-- Parameter: t: char
#-- Returns: int
#-- Description:
#--   Returns the index of target `t` in a `some` value or `none` if not found.
@pub fn find(@ref s, t) {
    for i in 0 to len(s) {
        if s[i] == t {
            return some(i);
        }
    }
    return none;
}
### End

### Function
#-- Name: trim
#-- Parameter: s: @ref str
#-- Returns: unit
#-- Description:
#--   Trims all whitespace (spaces, tabs, newlines etc.) from `s` in-place.
@pub fn trim(@ref s) {
    while len(s) > 0 &&
        (s[0] == ' '
        || s[0] == '\n'
        || s[0] == '\t'
        || s[0] == '\r') {
        s.pop(0);
    }
    while len(s) > 0 &&
        (s.back() == ' '
        || s.back() == '\n'
        || s.back() == '\t'
        || s.back() == '\r') {
        s.pop(len(s)-1);
    }
}
### End

### Function
#-- Name: find_first_of
#-- Parameter: s: @const @ref str
#-- Parameter: t: char
#-- Returns: option<int>
#-- Description:
#--   Finds the first ocurrence of `t` in `s`.
@pub fn find_first_of(@const @ref s, t) {
    for i in 0 to len(s) {
        if s[i] == t {
            return some(i);
        }
    }
    return none;
}
### End

@pub fn remove_all_of_char(@ref s: str, target: char): unit {
    let i = 0;
    while i < len(s) {
        if s[i] == target {
            s.pop(i);
        }
        else {
            i += 1;
        }
    }
}

### Function
#-- Name: find_last_of
#-- Parameter: s: @const @ref str
#-- Parameter: t: char
#-- Returns: option<int>
#-- Description:
#--   Finds the last ocurrence of `t` in `s`.
@pub fn find_last_of(@const @ref s, @const @ref t) {
    let idx = none;
    for i in 0 to len(s) {
        if s[i] == t {
            idx = some(i);
        }
    }
    return idx;
}
### End

### Function
#-- Name: is_only_ignorable
#-- Parameter: s: @const @ref str
#-- Returns: bool
#-- Description:
#--   Checks to see if the all characters in
#--   the string `s` are (` `, `\n`, `\r`, `\t`).
#--   Return `true` if all are one of the ignorable
#--   characters, or `false` if otherwise.
@pub fn is_only_ignorable(@const @ref s: str) {
    foreach c in s {
        if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
            return false;
        }
    }
    return true;
}
### End
