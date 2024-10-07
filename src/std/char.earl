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

module Char

### Function
#-- Name: isalpha
#-- Parameter: c: char
#-- Returns: bool
#-- Description:
#--  Takes the char `c` and returns a boolean of whether or not it is a an alpha character.
@pub fn isalpha(c) {
    let dc = c.ascii();
    return (dc >= 'A'.ascii() && dc <= 'Z'.ascii()) || (dc >= 'a'.ascii() && dc <= 'z'.ascii());
}
### End

### Function
#-- Name: isnum
#-- Parameter: c: char
#-- Returns: bool
#-- Description:
#--   Takes the char `c` and returns a boolean of whether or not
#--   it is a digit.
@pub fn isnum(c) {
    let dc = c.ascii();
    return dc >= '0'.ascii() && dc <= '9'.ascii();
}
### End

### Function
#-- Name: isalnum
#-- Parameter: c: char
#-- Returns: bool
#-- Description:
#--   Takes the char `c` and returns a boolean of whether or not it is a digit or an alpha character.
@pub fn isalnum(c) {
    return isalpha(c) || isnum(c);
}
### End

### Function
#-- Name: tolower
#-- Parameter: c: char
#-- Returns: char
#-- Description:
#--   Takes the char `c` and returns a lowercase character version of the latin alphabet
@pub fn tolower(c) {
    if isalpha(c) {
        # Dumb solution for now...
        match c {
            'A' -> { return 'a'; }
            'B' -> { return 'b'; }
            'C' -> { return 'c'; }
            'D' -> { return 'd'; }
            'E' -> { return 'e'; }
            'F' -> { return 'f'; }
            'G' -> { return 'g'; }
            'H' -> { return 'h'; }
            'I' -> { return 'i'; }
            'J' -> { return 'j'; }
            'K' -> { return 'k'; }
            'L' -> { return 'l'; }
            'M' -> { return 'm'; }
            'N' -> { return 'n'; }
            'O' -> { return 'o'; }
            'P' -> { return 'p'; }
            'Q' -> { return 'q'; }
            'R' -> { return 'r'; }
            'S' -> { return 's'; }
            'T' -> { return 't'; }
            'U' -> { return 'u'; }
            'V' -> { return 'v'; }
            'W' -> { return 'w'; }
            'X' -> { return 'x'; }
            'Y' -> { return 'y'; }
            'Z' -> { return 'z'; }
            _ -> { return c; }
        }
    }
    return c;
}
### End

### Function
#-- Name: toupper
#-- Parameter: c: char
#-- Returns: char
#-- Description:
#--  Takes the char `c` and returns a uppercase character version of the latin alphabet
@pub fn toupper(c) {
    if isalpha(c) {
        # Dumb solution for now...
        match c {
            'a' -> { return 'A'; }
            'b' -> { return 'B'; }
            'c' -> { return 'C'; }
            'd' -> { return 'D'; }
            'e' -> { return 'E'; }
            'f' -> { return 'F'; }
            'g' -> { return 'G'; }
            'h' -> { return 'H'; }
            'i' -> { return 'I'; }
            'j' -> { return 'J'; }
            'k' -> { return 'K'; }
            'l' -> { return 'L'; }
            'm' -> { return 'M'; }
            'n' -> { return 'N'; }
            'o' -> { return 'O'; }
            'p' -> { return 'P'; }
            'q' -> { return 'Q'; }
            'r' -> { return 'R'; }
            's' -> { return 'S'; }
            't' -> { return 'T'; }
            'u' -> { return 'U'; }
            'v' -> { return 'V'; }
            'w' -> { return 'W'; }
            'x' -> { return 'X'; }
            'y' -> { return 'Y'; }
            'z' -> { return 'Z'; }
            _ -> { return c; }
        }
    }
    return c;
}
### End
