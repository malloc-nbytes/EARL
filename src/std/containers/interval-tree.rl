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

module IntervalTree

class Interval [low, high] {
    @pub let low, high = (low, high);
}

class Node [range, max] {
    @pub let range, max, left, right = (
        range, max,
        none, none,
    );
}

### Class
#-- Name: T
#-- Parameter: init: list<tuple<real, real>>
#-- Description:
#--   Creates a new Interval Tree container.
@pub class T [init: list] {
    let root = none;

    fn constructor() {
        foreach l, h in init {
            this.insert(l, h);
        }
    }

    fn __insert(node, x) {
        if !node {
            return some(Node(x, x.high));
        }
        if x.low < node.unwrap().range.low {
            node.unwrap().left = __insert(node.unwrap().left, x);
        }
        else {
            node.unwrap().right = __insert(node.unwrap().right, x);
        }

        if node.unwrap().max < x.high {
            node.unwrap().max = x.high;
        }

        return node;
    }

    fn __overlapping(node, x) {
        if !node {
            return none;
        }

        if (x.low > node.unwrap().range.low && x.low < node.unwrap().range.high)
            || (x.high > node.unwrap().range.low && x.high < node.unwrap().range.high) {
            return some((node.unwrap().range.low, node.unwrap().range.high));
        }
        else if node.unwrap().left != none && node.unwrap().left.unwrap().max > x.low {
            return this.__overlapping(node.unwrap().left, x);
        }
        return this.__overlapping(node.unwrap().right, x);
    }

    ### Method
    #-- Name: insert
    #-- Parameter: low: real
    #-- Parameter: high: real
    #-- Returns: unit
    #-- Description:
    #--   Insert a new Interval into the tree.
    @pub fn insert(low, high): unit {
        this.root = this.__insert(this.root, Interval(low, high));
    }
    ### End

    ### Method
    #-- Name: overlapping
    #-- Parameter: low: real
    #-- Parameter: high: real
    #-- Returns: option<tuple<real, real>>
    #-- Description:
    #--   Check if the given interval `low`..`high` is contained
    #--   within another interval. If there are several, returns the
    #--   widest interval range.
    @pub fn overlapping(low, high): option {
        return this.__overlapping(this.root, Interval(low, high));
    }
    ### End
}
### End
