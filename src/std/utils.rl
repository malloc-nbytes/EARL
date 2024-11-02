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

module Utils

### Function
#-- Name: is_even
#-- Parameter: n: int
#-- Returns: bool
#-- Description:
#--   Returns `true` if `n` is even, or `false` if otherwise.
@pub fn is_even(n) {
    return n%2 == 0;
}
### End

### Function
#-- Name: is_odd
#-- Parameter: n: int
#-- Returns: bool
#-- Description:
#--   Returns `true` if `n` is odd, or `false` if otherwise.
@pub fn is_odd(n) {
    return n%2 != 0;
}
### End

### Function
#-- Name: swap
#-- Parameter: x1: @ref any
#-- Parameter: x2: @ref type(x1)
#-- Returns: unit
#-- Description:
#--   Swap `x1` and `x2` in-place.
@pub fn swap(@ref x1, @ref x2) {
    let t = x1;
    x1 = x2;
    x2 = t;
}
### End

let iota_N = 0;

### Function
#-- Name: iota
#-- Returns: int
#-- Description:
#--   Returns 0 on first call, then 0+1 on second, then 0+1+1 on third etc.
@pub @world
fn iota() {
    let res = iota_N;
    iota_N = iota_N+1;
    return res;
}
### End
