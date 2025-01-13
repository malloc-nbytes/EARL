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

### IGNORE

module BasicLexer

import "std/datatypes/char.rl";
import "std/io.rl";

### Enum
#-- Name: TokenType
#-- Parameter: Intlit: int
#-- Parameter: Strlit: int
#-- Parameter: Charlit: int
#-- Parameter: Eof: int
#-- Parameter: LParen: int
#-- Parameter: RParen: int
#-- Parameter: LCurlyBracket: int
#-- Parameter: RCurlyBracket: int
#-- Parameter: LSquareBracket: int
#-- Parameter: RSquareBracket: int
#-- Parameter: Greaterthan: int
#-- Parameter: Lessthan: int
#-- Parameter: Equals: int
#-- Parameter: Semicolon: int
#-- Parameter: Tilde: int
#-- Parameter: Backtick: int
#-- Parameter: Bang: int
#-- Parameter: At: int
#-- Parameter: Hash: int
#-- Parameter: Dollar: int
#-- Parameter: Percent: int
#-- Parameter: Uptick: int
#-- Parameter: Ampersand: int
#-- Parameter: Asterisk: int
#-- Parameter: Quote: int
#-- Parameter: SingleQuote: int
#-- Parameter: Colon: int
#-- Parameter: Comma: int
#-- Parameter: Period: int
#-- Parameter: ForwardSlash: int
#-- Parameter: BackwardSlash: int
#-- Parameter: QuestionMark: int
#-- Parameter: Hyphen: int
#-- Parameter: Newline: int
#-- Parameter: Plus: int
#-- Parameter: Pipe: int
#-- Description:
#--   The different token types.
@pub enum TokenType {
    Intlit = 0,
    Strlit,
    Charlit,
    Eof,
    LParen,
    RParen,
    LCurlyBracket,
    RCurlyBracket,
    LSquareBracket,
    RSquareBracket,
    Greaterthan,
    Lessthan,
    Equals,
    Semicolon,
    Tilde,
    Backtick,
    Bang,
    At,
    Hash,
    Dollar,
    Percent,
    Uptick,
    Ampersand,
    Asterisk,
    Quote,
    SingleQuote,
    Colon,
    Comma,
    Period,
    ForwardSlash,
    BackwardSlash,
    QuestionMark,
    Hyphen,
    Newline,
    Plus,
    Pipe,
}
### End

### Variable
#-- Name: EOF
#-- Type: @const int
#-- Description:
#--   The TokenType of end of file.
@pub @const let EOF = TokenType.Eof;
### End

### Class
#-- Name: Token
#-- Parameter: lx: str
#-- Parameter: ty: int
#-- Parameter: row: int
#-- Parameter: col: int
#-- Description:
#--   A class representing an individual token.
@pub class Token [lx: str, ty: int, row: int, col: int] {
    @pub let lx = lx;
    @pub let ty = ty;
    @pub let row = row;
    @pub let col = col;

    ### Method
    #-- Name: ttype
    #-- Returns: int
    #-- Description:
    #--   Get the type of the token.
    @pub fn ttype(): int {
        return this.ty;
    }
    ### End

    ### Method
    #-- Name: lexeme
    #-- Returns: str
    #-- Description:
    #--   Get the lexeme of the token.
    @pub fn lexeme(): str {
        return this.lx;
    }
    ### End

    ### Method
    #-- Name: get_row
    #-- Returns: int
    #-- Description:
    #--   Get the row of the token.
    @pub fn get_row(): int {
        return this.row;
    }
    ### End

    ### Method
    #-- Name: get_col
    #-- Returns: int
    #-- Description:
    #--   Get the column of the token.
    @pub fn get_col(): int {
        return this.col;
    }
    ### End
}
### End

### Class
#-- Name: T
#-- Parameter: src: str
#-- Description:
#--   The basic lexer class. Expects source code `s`,
#--   or give an empty string and use the method
#--   `lex()` later.
@pub @experimental
class T [src: str] {
    let src, tokens, syms = (
        src, [],
        {
            '(': TokenType.LParen,
            ')': TokenType.RParen,
            '{': TokenType.LCurlyBracket,
            '}': TokenType.RCurlyBracket,
            '[': TokenType.LSquareBracket,
            ']': TokenType.RSquareBracket,
            '>': TokenType.Greaterthan,
            '<': TokenType.Lessthan,
            '=': TokenType.Equals,
            ';': TokenType.Semicolon,
            '~': TokenType.Tilde,
            '`': TokenType.Backtick,
            '!': TokenType.Bang,
            '@': TokenType.At,
            '#': TokenType.Hash,
            '$': TokenType.Dollar,
            '%': TokenType.Percent,
            '^': TokenType.Uptick,
            '&': TokenType.Ampersand,
            '*': TokenType.Asterisk,
            '"': TokenType.Quote,
            '\'': TokenType.SingleQuote,
            ':': TokenType.Colon,
            ',': TokenType.Comma,
            '.': TokenType.Period,
            '/': TokenType.ForwardSlash,
            '\\': TokenType.BackwardSlash,
            '?': TokenType.QuestionMark,
            '-': TokenType.Hyphen,
            '+': TokenType.Plus,
            '|': TokenType.Pipe
        }
    );

    ### Method
    #-- Name: peek
    #-- Returns: option<Token>
    #-- Description:
    #--   Peek the current token. If there is none,
    #--   will return `none`. Otherwise `some(Token)`.
    @pub fn peek(): option {
        return case len(this.tokens) > 0 of {
            true = some(this.tokens[0]);
            _ = none;
        };
    }
    ### End

    ### Method
    #-- Name: next
    #-- Returns: Token
    #-- Description:
    #--   Get the next token. Will panic if there
    #--   are no tokens left.
    @pub fn next(): Token {
        if len(this.tokens) <= 0 {
            panic(__FILE__, ':', __FUNC__, ": ", "tried to call next() with no more tokens");
        }
        @ref let t = this.tokens[0];
        this.tokens.pop(0);
        return t;
    }
    ### End

    ### Method
    #-- Name: reset
    #-- Returns: unit
    #-- Description:
    #--   Reset the lexer.
    @pub fn reset(): unit {
        this.src = "";
        this.tokens = [];
    }
    ### End

    ### Method
    #-- Name: lex
    #-- Parameter: s: @const @ref str
    #-- Returns: unit
    #-- Description:
    #--   Lex the source code string `s`.
    @pub fn lex(@const @ref s: str): unit {
        this.src = s;
        this.tokens = [];
        this.__lex();
    }
    ### End

    fn __lex() {
        let row, col = (1, 1);
        with i = 0 in
        while i < len(src) {
            with c = src[i] in
            if c == '\n' {
                i += 1;
                col = 1;
                row += 1;
                this.tokens.append(Token("\n", TokenType.Newline, row, col));
            }
            else if c == ' ' || c == '\t' || c == '\r' {
                i += 1;
                col += 1;
            }
            else if c == '"' {
                i += 1;
                with s = this.consume_until(i, col, (== '"')) in
                this.tokens.append(Token(s, TokenType.Strlit, row, col));
                i += 1;
                row += i;
            }
            else if c == '\'' {
                i += 1;
                with s = this.consume_until(i, col, (== '\'')) in
                this.tokens.append(Token(s, TokenType.Charlit, row, col));
                i += 1;
                row += i;
            }
            else if Char::isnum(c) {
                with s = this.consume_until(i, col, |k| { return !Char::isnum(k); }) in
                this.tokens.append(Token(s, TokenType.Intlit, row, col));
            }
            else if this.syms[c] {
                this.tokens.append(Token(str(c), this.syms[c].unwrap(), row, col));
                i += 1;
                row += 1;
            }
            else if Char::isalpha(c) || c == '_' {
                with s = this.consume_until(i, col, |k| { return !Char::isalnum(k) && k != '_'; }) in
                this.tokens.append(Token(s, TokenType.Intlit, row, col));
            }
            else {
                panic(f"unknown token: {c}");
            }
        }

        this.tokens.append(Token("EOF", TokenType.Eof, row, col));
    }

    fn consume_until(@ref i, @ref col, predicate) {
        let s, esc = ("", false);
        while i < len(this.src) && (!esc && !predicate(this.src[i])) {
            let c = this.src[i];
            if c == '\\' {
                esc = true;
            }
            else {
                s += c;
                if esc {
                    esc = false;
                }
            }
            i += 1;
            col += 1;
        }
        return s;
    }

    fn constructor() {
        this.__lex();
    }
}
### End


### Function
#-- Name: from_fp
#-- Parameter: fp: str
#-- Returns: T
#-- Description:
#--   Create a `Lexer` from the source code in
#--   the file located at the filepath `fp`.
@pub @experimental
fn from_fp(fp: str): T {
    return T(IO::file_to_str(fp));
}
### End
