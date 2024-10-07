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

module Encryption

### Function
#-- Name: rle
#-- Parameter: src: str
#-- Returns: str
#-- Description:
#--   Performs Run Length Encoding on `src`.
@pub fn rle(src) {
    let result = "";
    let cur, count = (none, 1);
    for i in 0 to len(src) {
        if cur {
            if src[i] == cur.unwrap() {
                count += 1;
            }
            else {
                result.append(str(cur.unwrap()) + str(count));
                count = 1;
                cur = some(src[i]);
            }
        }
        else {
            cur = some(src[i]);
        }
    }

    if cur {
        result.append(str(cur.unwrap()) + str(count));
        count = 1;
    }

    return result;
}
### End

