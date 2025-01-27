/** file */

// MIT License

// Copyright (c) 2025 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <assert.h>
#include <string.h>

#include "utils.h"
#include "token.h"

static token_t *__token_alloc(const char *lx,
                              size_t len,
                              token_type_t type,
                              size_t row,
                              size_t col) {
    token_t *tok
        = (token_t *)s_malloc(sizeof(token_t), NULL, NULL);

    tok->lx = (char *)s_malloc(len + 1, NULL, NULL);
    (void)strncpy(tok->lx, lx, len);
    tok->lx[len] = '\0';

    tok->type = type;
    tok->row = row;
    tok->col = col;
    tok->next = NULL;

    return tok;
}

token_t *token_alloc(const char *lx,
                     token_type_t type,
                     size_t row,
                     size_t col) {
    return __token_alloc(lx, strlen(lx), type, row, col);
}

token_t *token_alloc_from_substr(const char *start,
                                 size_t substr_len,
                                 token_type_t type,
                                 size_t row,
                                 size_t col) {
    return __token_alloc(start, substr_len, type, row, col);
}

const char *token_type_to_cstr(token_type_t type) {
    if (type == TOKEN_TYPE_LEFT_PARENTHESIS) { return "TOKEN_TYPE_LEFT_PARENTHESIS"; }
    if (type == TOKEN_TYPE_RIGHT_PARENTHESIS) { return "TOKEN_TYPE_RIGHT_PARENTHESIS"; }
    if (type == TOKEN_TYPE_LEFT_CURLY_BRACKET) { return "TOKEN_TYPE_LEFT_CURLY_BRACKET"; }
    if (type == TOKEN_TYPE_RIGHT_CURLY_BRACKET) { return "TOKEN_TYPE_RIGHT_CURLY_BRACKET"; }
    if (type == TOKEN_TYPE_LEFT_SQUARE_BRACKET) { return "TOKEN_TYPE_LEFT_SQUARE_BRACKET"; }
    if (type == TOKEN_TYPE_RIGHT_SQUARE_BRACKET) { return "TOKEN_TYPE_RIGHT_SQUARE_BRACKET"; }
    if (type == TOKEN_TYPE_HASH) { return "TOKEN_TYPE_HASH"; }
    if (type == TOKEN_TYPE_PERIOD) { return "TOKEN_TYPE_PERIOD"; }
    if (type == TOKEN_TYPE_SEMICOLON) { return "TOKEN_TYPE_SEMICOLON"; }
    if (type == TOKEN_TYPE_COMMA) { return "TOKEN_TYPE_COMMA"; }
    if (type == TOKEN_TYPE_GREATERTHAN) { return "TOKEN_TYPE_GREATERTHAN"; }
    if (type == TOKEN_TYPE_LESSTHAN) { return "TOKEN_TYPE_LESSTHAN"; }
    if (type == TOKEN_TYPE_EQUALS) { return "TOKEN_TYPE_EQUALS"; }
    if (type == TOKEN_TYPE_AMPERSAND) { return "TOKEN_TYPE_AMPERSAND"; }
    if (type == TOKEN_TYPE_ASTERISK) { return "TOKEN_TYPE_ASTERISK"; }
    if (type == TOKEN_TYPE_PLUS) { return "TOKEN_TYPE_PLUS"; }
    if (type == TOKEN_TYPE_MINUS) { return "TOKEN_TYPE_MINUS"; }
    if (type == TOKEN_TYPE_FORWARD_SLASH) { return "TOKEN_TYPE_FORWARD_SLASH"; }
    if (type == TOKEN_TYPE_PIPE) { return "TOKEN_TYPE_PIPE"; }
    if (type == TOKEN_TYPE_CARET) { return "TOKEN_TYPE_CARET"; }
    if (type == TOKEN_TYPE_QUESTION_MARK) { return "TOKEN_TYPE_QUESTION_MARK"; }
    if (type == TOKEN_TYPE_BACKWARD_SLASH) { return "TOKEN_TYPE_BACKWARD_SLASH"; }
    if (type == TOKEN_TYPE_BANG) { return "TOKEN_TYPE_BANG"; }
    if (type == TOKEN_TYPE_AT) { return "TOKEN_TYPE_AT"; }
    if (type == TOKEN_TYPE_DOLLAR_SIGN) { return "TOKEN_TYPE_DOLLAR_SIGN"; }
    if (type == TOKEN_TYPE_PERCENT) { return "TOKEN_TYPE_PERCENT"; }
    if (type == TOKEN_TYPE_BACKTICK) { return "TOKEN_TYPE_BACKTICK"; }
    if (type == TOKEN_TYPE_TILDE) { return "TOKEN_TYPE_TILDE"; }
    if (type == TOKEN_TYPE_COLON) { return "TOKEN_TYPE_COLON"; }
    if (type == TOKEN_TYPE_DOUBLE_AMPERSAND) { return "TOKEN_TYPE_DOUBLE_AMPERSAND"; }
    if (type == TOKEN_TYPE_DOUBLE_PIPE) { return "TOKEN_TYPE_DOUBLE_PIPE"; }
    if (type == TOKEN_TYPE_GREATERTHAN_EQUALS) { return "TOKEN_TYPE_GREATERTHAN_EQUALS"; }
    if (type == TOKEN_TYPE_LESSTHAN_EQUALS) { return "TOKEN_TYPE_LESSTHAN_EQUALS"; }
    if (type == TOKEN_TYPE_DOUBLE_EQUALS) { return "TOKEN_TYPE_DOUBLE_EQUALS"; }
    if (type == TOKEN_TYPE_BANG_EQUALS) { return "TOKEN_TYPE_BANG_EQUALS"; }
    if (type == TOKEN_TYPE_PLUS_EQUALS) { return "TOKEN_TYPE_PLUS_EQUALS"; }
    if (type == TOKEN_TYPE_MINUS_EQUALS) { return "TOKEN_TYPE_MINUS_EQUALS"; }
    if (type == TOKEN_TYPE_ASTERISK_EQUALS) { return "TOKEN_TYPE_ASTERISK_EQUALS"; }
    if (type == TOKEN_TYPE_FORWARD_SLASH_EQUALS) { return "TOKEN_TYPE_FORWARD_SLASH_EQUALS"; }
    if (type == TOKEN_TYPE_PERCENT_EQUALS) { return "TOKEN_TYPE_PERCENT_EQUALS"; }
    if (type == TOKEN_TYPE_RIGHT_ARROW) { return "TOKEN_TYPE_RIGHT_ARROW"; }
    if (type == TOKEN_TYPE_DOUBLE_PERIOD) { return "TOKEN_TYPE_DOUBLE_PERIOD"; }
    if (type == TOKEN_TYPE_DOUBLE_COLON) { return "TOKEN_TYPE_DOUBLE_COLON"; }
    if (type == TOKEN_TYPE_DOUBLE_LESSTHAN) { return "TOKEN_TYPE_DOUBLE_LESSTHAN"; }
    if (type == TOKEN_TYPE_DOUBLE_GREATERTHAN) { return "TOKEN_TYPE_DOUBLE_GREATERTHAN"; }
    if (type == TOKEN_TYPE_DOUBLE_ASTERISK) { return "TOKEN_TYPE_DOUBLE_ASTERISK"; }
    if (type == TOKEN_TYPE_PIPE_GREATERTHAN) { return "TOKEN_TYPE_PIPE_GREATERTHAN"; }
    if (type == TOKEN_TYPE_BACKTICK_PIPE) { return "TOKEN_TYPE_BACKTICK_PIPE"; }
    if (type == TOKEN_TYPE_BACKTICK_AMPERSAND) { return "TOKEN_TYPE_BACKTICK_AMPERSAND"; }
    if (type == TOKEN_TYPE_BACKTICK_TILDE) { return "TOKEN_TYPE_BACKTICK_TILDE"; }
    if (type == TOKEN_TYPE_BACKTICK_CARET) { return "TOKEN_TYPE_BACKTICK_CARET"; }
    if (type == TOKEN_TYPE_BACKTICK_PIPE_EQUALS) { return "TOKEN_TYPE_BACKTICK_PIPE_EQUALS"; }
    if (type == TOKEN_TYPE_BACKTICK_AMPERSAND_EQUALS) { return "TOKEN_TYPE_BACKTICK_AMPERSAND_EQUALS"; }
    if (type == TOKEN_TYPE_BACKTICK_CARET_EQUALS) { return "TOKEN_TYPE_BACKTICK_CARET_EQUALS"; }
    if (type == TOKEN_TYPE_IDENTIFIER) { return "TOKEN_TYPE_IDENTIFIER"; }
    if (type == TOKEN_TYPE_STRING_LITERAL) { return "TOKEN_TYPE_STRING_LITERAL"; }
    if (type == TOKEN_TYPE_INTEGER_LITERAL) { return "TOKEN_TYPE_INTEGER_LITERAL"; }
    if (type == TOKEN_TYPE_CHARACTER_LITERAL) { return "TOKEN_TYPE_CHARACTER_LITERAL"; }
    if (type == TOKEN_TYPE_HEXADECIMAL_LITERAL) { return "TOKEN_TYPE_HEXADECIMAL_LITERAL"; }
    if (type == TOKEN_TYPE_FLOATING_POINT_LITERAL) { return "TOKEN_TYPE_FLOATING_POINT_LITERAL"; }
    if (type == TOKEN_TYPE_TYPE_LITERAL) { return "TOKEN_TYPE_TYPE_LITERAL"; }
    if (type == TOKEN_TYPE_KEYWORD) { return "TOKEN_TYPE_KEYWORD"; }
    if (type == TOKEN_TYPE_INFO) { return "TOKEN_TYPE_INFO"; }
    if (type == TOKEN_TYPE_MULTILINE_BASH) { return "TOKEN_TYPE_MULTILINE_BASH"; }
    if (type == TOKEN_TYPE_BASH_LITERAL) { return "TOKEN_TYPE_BASH_LITERAL"; }
    if (type == TOKEN_TYPE_EOF) { return "TOKEN_TYPE_EOF"; }
    assert(0 && "unreachable");
}

