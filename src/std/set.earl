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

module Set

### Class
#-- Name: T
#-- Parameter: init: list<x0: any, x1: type(x0), ..., xN: type(x0)>
#-- Description:
#--   Creates a new Set container with the initializer list `init`.
@pub class T [init] {
    let items = none;
    let type_ = int;

    fn init(value) {
        this.type_ = typeof(value);
        items = some(Dict(this.type_));
    }

    fn constructor() {
        if len(init) != 0 {
            this.init(init[0]);
            for i in 0 to len(init) {
                this.insert(init[i]);
            }
        }
    }

    ### Method
    #-- Name: insert
    #-- Parameter: value: any
    #-- Returns: unit
    #-- Description:
    #--  Insert `value` into the `set`. A panic will occur
    #--  if the `typeof(value)` is not the same as the other
    #--  values in the `set`.
    @pub fn insert(value) {
        if !items {
            this.init(value);
        }

        if typeof(value) != type_ {
            panic(f"Set::T expected value `{type_}` but got: ", typeof(value));
        }

        if !this.items.unwrap()[value] {
            this.items
                    .unwrap()
                    .insert(value, _);
        }
    }
    ### End

    ### Method
    #-- Name: contains
    #-- Parameter: value: any
    #-- Returns: bool
    #-- Description:
    #--   Returns `true` if `value` is in the `set`, or `false` if it is not.
    #--   A panic will occur if the `typeof(value)` is not the same as the other
    #--   values in the `set`.
    @pub fn contains(value) {
        if typeof(value) != type_ {
            panic(f"Set::T expected value  `{type_}` but got: ", typeof(value));
        }

        if !this.items {
            return false;
        }

        return this.items
                       .unwrap()[value]
                       .is_some();
    }
    ### End
}
### End
