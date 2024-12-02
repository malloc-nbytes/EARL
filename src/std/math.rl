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

module Math

import "std/datatypes/list.rl";

### Function
#-- Name: max
#-- Parameter: x: real
#-- Parameter: y: real
#-- Returns: real
#-- Description:
#--   Returns the greater of `x` and `y`.
@pub fn max(x: real, y: real): real {
    if x >= y {
        return x;
    }
    return y;
}
### End

### Function
#-- Name: min
#-- Parameter: x: real
#-- Parameter: y: real
#-- Returns: real
#-- Description:
#--   Returns the minimum of `x` and `y`.
@pub fn min(x: real, y: real): real {
    if x <= y {
        return x;
    }
    return y;
}
### End

### Function
#-- Name: abs
#-- Parameter: x: real
#-- Returns: real
#-- Description:
#--   Returns the absolute value of `x`.
@pub fn abs(x: real): real {
    if x < 0 {
        return x * -1;
    }
    return x;
}
### End

# Statistical Functions #

### Function
#-- Name: mode
#-- Parameter: lst: list
#-- Returns: list
#-- Description:
#--   This function returns a list of all modes that appear at the
#--   highest frequency in the given list in the order modes are found.
@pub fn mode(lst) {

    if len(lst) == 0 {
        return none;
    }

    let count = 0;
    let max_count = 0;
    let lst_modes = [];

    for i in 0 to len(lst) {
        if (List::find(lst_modes, lst[i]) == -1) {

            count = List::count(lst, lst[i]);

            if max_count == count {
                lst_modes.append(lst[i]);
                count = 0;
            }

            if max_count < count {
                lst_modes = [];
                lst_modes.append(lst[i]);
                max_count = count;
                count = 0;
             }
        }
    }
    return lst_modes;
}
### End

### Function
#-- Name: mean
#-- Parameter: lst: list
#-- Returns: float
#-- Description:
#--   Returns the mean (average in a dataset) of a given list.
@pub fn mean(lst) {
    return List::sumf(lst) / len(lst);
}
### End

### Function
#-- Name: median
#-- Parameter: lst: list
#-- Returns: int
#-- Description:
#--   This function sorts and then returns the middle number of a given list
@pub fn median(lst) {
    List::quicksort(lst, List::DEFAULT_INT_ASCEND_QUICKSORT);

    let middle = len(lst)/2;

    if len(lst) % 2 == 0 {
        return (lst[middle-1]+lst[middle])/2.0;
    }

    return lst[middle];
}
### End

### Function
#-- Name: list_min
#-- Parameter: lst: list
#-- Returns: real
#-- Description:
#--   Returns the smallest element `lst`.
@pub fn list_min(lst: list): real {
    if len(lst) == 0 {
        return none;
    }

    let min = lst[0];

    for i in 1 to len(lst) {
        if lst[i] < min {
            min = lst[i];
        }
    }

    return min;
}
### End

### Function
#-- Name: list_max
#-- Parameter: lst: list
#-- Returns: real
#-- Description:
#--   Returns the largest element `lst`.
@pub fn list_max(lst: list): real {
    if len(lst) == 0 {
        return none;
    }

    let max = lst[0];

    for i in 1 to len(lst) {
        if lst[i] > max {
            max = lst[i];
        }
    }

    return max;
}
### End

### Function
#-- Name: area_of_circle
#-- Parameter: r: real
#-- Returns: real
#-- Description:
#--   Returns the area of a circle with radius `r`.
@pub @world
fn area_of_circle(r: real): real {
    return PI * (r**2);
}
### End

### Function
#-- Name: volume_of_cylinder
#-- Parameter: r: real
#-- Parameter: h: real
#-- Returns: real
#-- Description:
#--   Returns the volume of a cylinder with radius `r` and height `h`.
@pub @world
fn volume_of_cylinder(r: real, h: real): real {
    return h * PI * (r**2);
}
### End

### Function
#-- Name: volume_of_cone
#-- Parameter: r: real
#-- Parameter: h: real
#-- Returns: real
#-- Description:
#--   Returns the volume of a cone with radius `r` and height `h`.
@pub @world
fn volume_of_cone(r: real, h: real): real {
    return (1.0/3.0) * h * PI * (r**2);
}
### End

### Function
#-- Name: floor
#-- Parameter: f: float
#-- Returns: int
#-- Description:
#--   Returns the floor of `f`.
@pub fn floor(f: float): int {
    return int(f);
}
### End

### Function
#-- Name: ceil
#-- Parameter: f: float
#-- Returns: int
#-- Description:
#--   Returns the ceil of `f`.
@pub fn ceil(f: float): int {
    return floor(1+f);
}
### End

### Function
#-- Name: lerp
#-- Parameter: a: real
#-- Parameter: b: real
#-- Parameter: f: real
#-- Returns: real
#-- Description:
#--   Performs the lerp algorithm from `a` to `b` by `f`.
@pub fn lerp(a: real, b: real, f: real): float {
    return a*(1.-f)+(b*f);
}
### End

### Function
#-- Name: normalize
#-- Parameter: x: real
#-- Parameter: min: float
#-- Parameter: max: float
#-- Returns: float
#-- Description:
#--   Normalizes `x` to the range of `min` .. `max`.
@pub fn normalize(x: real, min: float, max: float): float {
    return (x-min)/(1-max);
}
### End

### Function
#-- Name: clamp
#-- Parameter: value: real
#-- Parameter: min: real
#-- Parameter: max: real
#-- Returns: real
#-- Description:
#--   Performs clamp on `value` with `min` and `max`.
@pub fn clamp(value: real, min: real, max: real): real {
    return max(min, min(value, max));
}
### End

### Variable
#-- Name: PI
#-- Type: float
#-- Description:
#--   The mathematical constant for pi.
@pub @const let PI = 3.141592653589793;
### End

### Variable
#-- Name: EULER
#-- Type: float
#-- Description:
#--   The mathematical constant for Euler's number.
@pub @const let EULER = 2.718281828459045;
### End
