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

module Stack

### Class
#-- Name: T
#-- Parameter: init: list
#-- Description:
#--   A stack data structure.
class T [init] {
    let lst = init;

    ### Method
    #-- Name: push
    #-- Parameter: val: any
    #-- Returns: unit
    #-- Description:
    #--   Pushes a value to the top of the stack.
    @pub fn push(val) {
        this.lst.append(val);
    }
    ### End

    ### Method
    #-- Name: pop
    #-- Returns: unit
    #-- Description:
    #--   Pops the top value from the stack.
    @pub fn pop() {
        if len(this.lst) == 0 {
            panic(__FILE__, ':', __FUNC__, ": ", "Stack is empty");
        }
        this.lst.pop(len(this.lst) - 1);
    }
    ### End

    ### Method
    #-- Name: top
    #-- Returns: any
    #-- Description:
    #--   Returns the top value of the stack.
    @pub fn top() {
        if len(this.lst) == 0 {
            panic(__FILE__, ':', __FUNC__, ": ", "Stack is empty");
        }
        this.lst[len(this.lst) - 1];
    }
    ### End

    ### Method
    #-- Name: clear
    #-- Returns: unit
    #-- Description:
    #--   Clears the stack.
    @pub fn clear() {
        this.lst = [];
    }
    ### End

    ### Method
    #-- Name: empty
    #-- Returns: bool
    #-- Description:
    #--   Returns true if the stack is empty.
    @pub fn clear() {
        this.lst = [];
    }
    ### End
}
### End