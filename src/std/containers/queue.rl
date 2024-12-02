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

module Queue

### Class
#-- Name: T
#-- Parameter: init: list
#-- Description:
#--   A queue data structure.
@pub class T [init: list] {
    let lst = init;

    ### Method
    #-- Name: push
    #-- Parameter: val: any
    #-- Returns: unit
    #-- Description:
    #--   Pushes a value to the back of the queue.
    @pub fn push(val) {
        this.lst.append(val);
    }
    ### End

    ### Method
    #-- Name: pop
    #-- Returns: unit
    #-- Description:
    #--   Pops the front value from the queue.
    @pub fn pop() {
        if len(this.lst) == 0 {
            panic(__FILE__, ':', __FUNC__, ": ", "Queue is empty");
        }
        this.lst.pop(0);
    }
    ### End

    ### Method
    #-- Name: front
    #-- Returns: any
    #-- Description:
    #--   Returns the front value of the queue.
    @pub fn front() {
        if len(this.lst) == 0 {
            panic(__FILE__, ':', __FUNC__, ": ", "Queue is empty");
        }
        this.lst[0];
    }
    ### End

    ### Method
    #-- Name: empty
    #-- Returns: bool
    #-- Description:
    #--   Returns true if the queue is empty.
    @pub fn empty() {
        len(this.lst) == 0;
    }
    ### End

    ### Method
    #-- Name: size
    #-- Returns: int
    #-- Description:
    #--   Returns the size of the queue.
    @pub fn size() {
        len(this.lst);
    }
    ### End

    ### Method
    #-- Name: clear
    #-- Returns: unit
    #-- Description:
    #--   Clears the queue.
    @pub fn clear() {
        this.lst = [];
    }
    ### End
}
### End
