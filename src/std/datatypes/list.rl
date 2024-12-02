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

module List

import "std/utils.rl"; full

### Function
#-- Name: preset
#-- Parameter: elem: any
#-- Parameter: cap: int
#-- Returns: list
#-- Description:
#--   Creates a list with elements `elem` of size `cap`.
@pub fn preset(elem, cap) {
    let lst = [];
    for i in 0 to cap {
        lst.append(elem);
    }
    return lst;
}
### End

### Function
#-- Name: from_range
#-- Parameter: start: int
#-- Parameter: stop: int
#-- Parameter: stride: int
#-- Returns: list<int|float>
#-- Description:
#--   Takes a list and then returns element within range specified by the parameters `start`, `stop`, `stride`.
@pub fn from_range(start, stop, stride) {
    if stride == 0 {
        panic(__FILE__, ":", __FUNC__, ": parameter `stride` must not be 0");
    }

    let lst = [];
    let i = start;

    while i <= stop {
        lst.append(i);
        i = i+stride;
    }

    return lst;
}
### End

### Function
#-- Name: fill
#-- Parameter: lst: @ref list<any>
#-- Parameter: k: any
#-- Returns: unit
#-- Description:
#--   Fills the given list `lst` with element `k`.
@pub fn fill(@ref lst, k) {
    for i in 0 to len(lst) {
        lst[i] = k;
    }
}
### End

### Function
#-- Name: sumf
#-- Parameter: lst: @const @ref list<real>
#-- Returns: float
#-- Description:
#--   Returns the sum all elements in `lst` as a float.
@pub fn sumf(@const @ref lst) {
    let s = 0.;
    for i in 0 to len(lst) {
        s += lst[i];
    }
    return s;
}
### End

### Function
#-- Name: sum
#-- Parameter: lst: @const @ref list<int|float>
#-- Returns: int
#-- Description:
#--   Returns the sum all elements in `lst` as an integer.
@pub fn sum(@const @ref lst) {
    let s = 0;
    for i in 0 to len(lst) {
        s += lst[i];
    }
    return s;
}
### End

### Function
#-- Name: find
#-- Parameter: lst: @const @ref list<any>
#-- Parameter: elem: @const @ref any
#-- Returns: option<int>
#-- Description:
#--   Takes a reference to a list and a reference to an element and looks for the element find in the given list
#--   Returns the index of the first occurrence that `elem` appears in `lst` wrapped in `some`, or `none` if not found.
@pub fn find(@const @ref lst, @const @ref elem) {
    let i = 0;
    while i < len(lst) {
        if elem == lst[i] {
            return some(i);
        }
        i += 1;
    }
    return none;
}
### End

### Function
#-- Name: count
#-- Parameter: lst: @const @ref list<any>
#-- Parameter: elem: @const @ref any
#-- Returns: int
#-- Description:
#--   Counts the number of occurrences that `elem` appears in `lst`.
@pub fn count(@const @ref lst, @const @ref elem) {
    let c = 0;
    for i in 0 to len(lst) {
        if elem == lst[i] {
            c = c+1;
        }
    }
    return c;
}
### End

### Function
#-- Name: quicksort
#-- Parameter: lst: @ref list<any>
#-- Parameter: compar: @const closure(x1: any, x2: type(x1)) -> bool|int
#-- Returns: unit
#-- Description:
#--   where =compar= is some ordering function $F(x_1, x_2) \in \{true, false\}$
#--   and $F$ is defined by
#--   \[
#--   F(x_1, x_2) = \begin{cases}
#--         true & \text{if } R(x_1) \ge R(x_2) \\
#--         false & \text{if } R(x_1) < R(x_2)
#--      \end{cases}
#--   \]
#--   and $R(x)$ is some ranking function that produces a rank of $x$.
#--   Performs the quicksort sorting algorithm on =lst= and
#--   sorts by the comparison closure =compar=.
@pub fn quicksort(@ref lst, @const compar) {
    let __quicksort = |@ref lst, low, high, @ref compar| {
        let partition = |@ref lst, low, high| {
            let pivot = lst[high];
            let i = low-1;

            for j in low to high {
                if compar(lst[j], pivot) {
                    i = i+1;
                    Utils::swap(lst[i], lst[j]);
                }
            }

            Utils::swap(lst[i+1], lst[high]);

            return i+1;
        };

        if low < high {
            let pi = partition(lst, low, high);
            __quicksort(lst, low, pi-1, compar);
            __quicksort(lst, pi+1, high, compar);
        }
    };

    __quicksort(lst, 0, len(lst)-1, compar);
}
### End

### Function
#-- Name: dict_to_list
#-- Parameter: dict: @const @ref dict<any, any>
#-- Returns: list<tuple<any, any>>
#-- Description:
#--   Converts the dictionary `dict` to a list.
@pub fn dict_to_list(@const @ref dict) {
    let lst = [];

    foreach @const key, value in dict {
        lst.append((key, value));
    }

    return lst;
}
### End

### Variable
#-- Name: DEFAULT_INT_ASCEND_QUICKSORT
#-- Type: closure(int, int) -> bool
#-- Description:
#--   The default comparison function for ascending sort.
@pub let DEFAULT_INT_ASCEND_QUICKSORT = |x, y| { return y > x; };
### End

### Variable
#-- Name: DEFAULT_INT_DESCEND_QUICKSORT
#-- Type: closure(int, int) -> bool
#-- Description:
#--   The default comparison function for descending sort.
@pub let DEFAULT_INT_DESCEND_QUICKSORT = |x, y| { return x > y; };
### End
