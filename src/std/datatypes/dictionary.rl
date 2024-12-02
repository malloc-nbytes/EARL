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

module Dictionary

import "std/assert.rl";

Assert::FILE = __FILE__;

### Function
#-- Name: to_list
#-- Parameter: @const @ref d: dictionary
#-- Returns: list<tuple>
#-- Description
#--   Takes the dictionary `d` and converts it
#--   to a list of tuples.
@pub fn to_list(@const @ref d: dictionary): list {
    let lst = [];
    foreach k, v in d {
        lst.append((k, v));
    }
    return lst;
}
### End

### Function
#-- Name: of_list
#-- Parameter: @const @ref lst: list<x0: any, x1: typeof(x0), x2: typeof(x0), ..., xN: typeof(x0)>
#-- Returns: dictionary<typeof(lst[0])>
#-- Description:
#--   Takes a list where all values are the same type and
#--   converts it into a dictionary of the number of times
#--   some value appears in `lst`. Note: `lst` is expected
#--   to have a length > 0.
@pub fn of_list(@const @ref lst: list): dictionary {
    Assert::FUNC = __FUNC__;
    Assert::is_true(len(lst) > 0);

    let d = Dict(typeof(lst[0]));
    foreach k in lst {
        if d[k] {
            d[k].unwrap() += 1;
        }
        else {
            d.insert(k, 1);
        }
    }

    return d;
}
### End
