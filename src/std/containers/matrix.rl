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

module Matrix

### Class
#-- Name: T
#-- Parameter: init: list<any>
#-- Parameter: rows: int
#-- Parameter: cols: int
#-- Description:
#--   Creates a new matrix with the initial dataset `init`
#--   with `rows` rows and `cols` columns.
@pub class T [init: list, rows: int, cols: int] {
    let x = 1;
    let lst = init;
    let r, c = (rows, cols);

    ### Method
    #-- Name: at
    #-- Parameter: i: int
    #-- Parameter: j: int
    #-- Returns: any
    #-- Description:
    #--   Returns the element at [ `i` ][ `j` ] in the matrix.
    @pub fn at(i, j) {
        if i < 0 || i >= this.r || j < 0 || j >= this.c {
            panic(f"The index at [{i}][{j}] is out of range of matrix of size {r}x{c} (size ", this.c*this.r, ")");
        }
        return this.lst[i*this.c+j];
    }
    ### End

    ### Method
    #-- Name: show
    #-- Returns: unit
    #-- Description:
    #--   Prints all elements in the matrix.
    @pub fn show() {
        for i in 0 to this.r {
            for j in 0 to this.c {
                print(this.lst[i*this.c+j]);
                if j != this.c-1 {
                    print(' ');
                }
            }
            println();
        }
    }
    ### End
}
### End

### Function
#-- Name: identity
#-- Returns: T
#-- Description:
#--   Creates the identity matrix.
@pub fn identity(): T {
    return T([1,0,0,0,1,0,0,0,1], 3, 3);
}
### End

### Function
#-- Name: from1d
#-- Parameter: data: list<any>
#-- Parameter: rows: int
#-- Parameter: cols: int
#-- Returns: T
#-- Description:
#--   Creates a `rows` x `cols` matrix from a 1d list.
@pub fn from1d(data: list, rows: int, cols: int): T {
    return T(data, rows, cols);
}
### End

### Function
#-- Name: from2d
#-- Parameter: data: list<list<any>>
#-- Returns: T
#-- Description:
#--   Creates a matrix from a 2d list.
@pub fn from2d(lst: list): T {
    let rows = len(lst);
    let cols = len(lst[0]);
    let actual = [];

    foreach row in lst {
        if (len(row) != cols) {
            panic("Invalid 2d matrix. Found column where its length (", len(row), ") does not match the first column (", cols, ")");
        }
        foreach i in row {
            actual.append(i);
        }
    }

    return T(actual, rows, cols);
}
### End
