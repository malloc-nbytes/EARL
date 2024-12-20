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

module Colors

### Enum
#-- Name: Tfc
#-- Parameter: Black: str
#-- Parameter: Red: str
#-- Parameter: Green: str
#-- Parameter: Yellow: str
#-- Parameter: Blue: str
#-- Parameter: Magenta: str
#-- Parameter: Cyan: str
#-- Parameter: White: str
#-- Parameter: Bright_Black: str
#-- Parameter: Bright_Red: str
#-- Parameter: Bright_Green: str
#-- Parameter: Bright_Yellow: str
#-- Parameter: Bright_Blue: str
#-- Parameter: Bright_Magenta: str
#-- Parameter: Bright_Cyan: str
#-- Parameter: Bright_White: str
#-- Description:
#--   The different (T)ext (f)orground (c)olors.
@pub enum Tfc {
    Black           = "\033[30m",
    Red             = "\033[31m",
    Green           = "\033[32m",
    Yellow          = "\033[33m",
    Blue            = "\033[34m",
    Magenta         = "\033[35m",
    Cyan            = "\033[36m",
    White           = "\033[37m",
    Bright_Black    = "\033[90m",
    Bright_Red      = "\033[91m",
    Bright_Green    = "\033[92m",
    Bright_Yellow   = "\033[93m",
    Bright_Blue     = "\033[94m",
    Bright_Magenta  = "\033[95m",
    Bright_Cyan     = "\033[96m",
    Bright_White    = "\033[97m",
}
### End

### Enum
#-- Name: Tbc
#-- Parameter: Black: str
#-- Parameter: Red: str
#-- Parameter: Green: str
#-- Parameter: Yellow: str
#-- Parameter: Blue: str
#-- Parameter: Magenta: str
#-- Parameter: Cyan: str
#-- Parameter: White: str
#-- Parameter: Bright_Black: str
#-- Parameter: Bright_Red: str
#-- Parameter: Bright_Green: str
#-- Parameter: Bright_Yellow: str
#-- Parameter: Bright_Blue: str
#-- Parameter: Bright_Magenta: str
#-- Parameter: Bright_Cyan: str
#-- Parameter: Bright_White: str
#-- Description:
#--   The different (T)ext (b)ackground (c)olors.
@pub enum Tbc {
    Black           = "\033[40m",
    Red             = "\033[41m",
    Green           = "\033[42m",
    Yellow          = "\033[43m",
    Blue            = "\033[44m",
    Magenta         = "\033[45m",
    Cyan            = "\033[46m",
    White           = "\033[47m",
    Bright_Black    = "\033[100m",
    Bright_Red      = "\033[101m",
    Bright_Green    = "\033[102m",
    Bright_Yellow   = "\033[103m",
    Bright_Blue     = "\033[104m",
    Bright_Magenta  = "\033[105m",
    Bright_Cyan     = "\033[106m",
    Bright_White    = "\033[107m",
}
### End

### Enum
#-- Name: Te
#-- Parameter: Underline: str
#-- Parameter: Bold: str
#-- Parameter: Italic: str
#-- Parameter: Invert: str
#-- Parameter: Strikethrough: str
#-- Parameter: Reset: str
#-- Description:
#--   The different (T)ext (e)ffects.
@pub enum Te {
    Underline = "\033[4m",
    Bold = "\033[1m",
    Italic = "\033[3m",
    Invert = "\033[7m",
    Strikethrough = "\033[9m",
    Reset = "\033[0m",
}
### End

### Function
#-- Name: show_fg_colors
#-- Returns: unit
#-- Description:
#--   Show all text forground colors printed to `stdout`.
#--   This does reset all current color and effect terminal codes.
@pub fn show_fg_colors(): unit {
    print(Te.Reset);
    println(Tfc.Black, "Black");
    println(Tfc.Green, "Green");
    println(Tfc.Red, "Red");
    println(Tfc.Yellow, "Yellow");
    println(Tfc.Blue, "Blue");
    println(Tfc.Magenta, "Magenta");
    println(Tfc.Cyan, "Cyan");
    println(Tfc.White, "White");
    println(Tfc.Bright_Black, "Bright Black");
    println(Tfc.Bright_Red, "Bright Red");
    println(Tfc.Bright_Green, "Bright Green");
    println(Tfc.Bright_Yellow, "Bright Yellow");
    println(Tfc.Bright_Blue, "Bright Blue");
    println(Tfc.Bright_Magenta, "Bright Magenta");
    println(Tfc.Bright_Cyan, "Bright Cyan");
    println(Tfc.Bright_White, "Bright White");
    print(Te.Reset);
}
### End

### Function
#-- Name: show_bg_colors
#-- Returns: unit
#-- Description:
#--   Show all text background colors printed to `stdout`.
#--   This does reset all current color and effect terminal codes.
@pub fn show_bg_colors(): unit {
    print(Te.Reset);
    println(Tbc.Black, "Black");
    print(Tfc.Black);
    println(Tbc.Red, "Red");
    println(Tbc.Green, "Green");
    println(Tbc.Yellow, "Yellow");
    println(Tbc.Blue, "Blue");
    println(Tbc.Magenta, "Magenta");
    println(Tbc.Cyan, "Cyan");
    println(Tbc.White, "White");
    print(Tfc.White);
    println(Tbc.Bright_Black, "Bright Black");
    print(Tfc.Black);
    println(Tbc.Bright_Red, "Bright Red");
    println(Tbc.Bright_Green, "Bright Green");
    println(Tbc.Bright_Yellow, "Bright Yellow");
    println(Tbc.Bright_Blue, "Bright Blue");
    println(Tbc.Bright_Magenta, "Bright Magenta");
    println(Tbc.Bright_Cyan, "Bright Cyan");
    println(Tbc.Bright_White, "Bright White");
    print(Te.Reset);
}
### End

### Function
#-- Name: show_text_effects
#-- Returns: unit
#-- Description:
#--   Show all text effects colors printed to `stdout`.
#--   This does reset all current color and effect terminal codes.
@pub fn show_text_effects(): unit {
    println(Te.Reset, Te.Underline, "Underline");
    println(Te.Reset, Te.Bold, "Bold");
    println(Te.Reset, Te.Italic, "Italic");
    println(Te.Reset, Te.Invert, "Invert");
    println(Te.Reset, Te.Strikethrough, "Strikethrough");
    println(Te.Reset, Te.Reset, "Reset");
}
### End
