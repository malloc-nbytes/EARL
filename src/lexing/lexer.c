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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexing/token.h"
#include "lexing/lexer.h"
#include "global/keywords.h"
#include "misc/arena.h"

/// @brief Handle symbols during lexing
/// @param src The start of the symbol
/// @param row The current row
/// @param col The current column
/// @param i The current index during lexing
/// @return The new symbol token
static token_t *handle_symbol(const char *src, size_t *row, size_t *col, size_t *i) {
    // 3 length symbols
    if (*src == '`' && src[1] && src[1] == '|' && src[2] && src[2] == '=') {
        *i += 3, *col += 3;
        return token_alloc("`|=", TOKEN_TYPE_BACKTICK_PIPE_EQUALS, *row, *col);
    }
    if (*src == '`' && src[1] && src[1] == '&' && src[2] && src[2] == '=') {
        *i += 3, *col += 3;
        return token_alloc("`&=", TOKEN_TYPE_BACKTICK_AMPERSAND_EQUALS, *row, *col);
    }
    if (*src == '`' && src[1] && src[1] == '^' && src[2] && src[2] == '=') {
        *i += 3, *col += 3;
        return token_alloc("`^=", TOKEN_TYPE_BACKTICK_CARET_EQUALS, *row, *col);
    }

    // 2 length symbols
    if (*src == '`' && src[1] && src[1] == '|') {
        *i += 2, *col += 2;
        return token_alloc("`|", TOKEN_TYPE_BACKTICK_PIPE, *row, *col);
    }
    if (*src == '`' && src[1] && src[1] == '&') {
        *i += 2, *col += 2;
        return token_alloc("`&", TOKEN_TYPE_BACKTICK_AMPERSAND, *row, *col);
    }
    if (*src == '`' && src[1] && src[1] == '~') {
        *i += 2, *col += 2;
        return token_alloc("`~", TOKEN_TYPE_BACKTICK_TILDE, *row, *col);
    }
    if (*src == '`' && src[1] && src[1] == '^') {
        *i += 2, *col += 2;
        return token_alloc("`^", TOKEN_TYPE_BACKTICK_CARET, *row, *col);
    }
    if (*src == '&' && src[1] && src[1] == '&') {
        *i += 2, *col += 2;
        return token_alloc("&&", TOKEN_TYPE_DOUBLE_AMPERSAND, *row, *col);
    }
    if (*src == '|' && src[1] && src[1] == '|') {
        *i += 2, *col += 2;
        return token_alloc("||", TOKEN_TYPE_DOUBLE_PIPE, *row, *col);
    }
    if (*src == '>' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc(">=", TOKEN_TYPE_GREATERTHAN_EQUALS, *row, *col);
    }
    if (*src == '<' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("<=", TOKEN_TYPE_LESSTHAN_EQUALS, *row, *col);
    }
    if (*src == '=' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("==", TOKEN_TYPE_DOUBLE_EQUALS, *row, *col);
    }
    if (*src == '!' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("!=", TOKEN_TYPE_BANG_EQUALS, *row, *col);
    }
    if (*src == '+' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("+=", TOKEN_TYPE_PLUS_EQUALS, *row, *col);
    }
    if (*src == '-' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("-=", TOKEN_TYPE_MINUS_EQUALS, *row, *col);
    }
    if (*src == '*' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("*=", TOKEN_TYPE_ASTERISK_EQUALS, *row, *col);
    }
    if (*src == '/' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("/=", TOKEN_TYPE_FORWARD_SLASH_EQUALS, *row, *col);
    }
    if (*src == '%' && src[1] && src[1] == '=') {
        *i += 2, *col += 2;
        return token_alloc("%=", TOKEN_TYPE_PERCENT_EQUALS, *row, *col);
    }
    if (*src == '-' && src[1] && src[1] == '>') {
        *i += 2, *col += 2;
        return token_alloc("->", TOKEN_TYPE_RIGHT_ARROW, *row, *col);
    }
    if (*src == '.' && src[1] && src[1] == '.') {
        *i += 2, *col += 2;
        return token_alloc("..", TOKEN_TYPE_DOUBLE_PERIOD, *row, *col);
    }
    if (*src == ':' && src[1] && src[1] == ':') {
        *i += 2, *col += 2;
        return token_alloc("::", TOKEN_TYPE_DOUBLE_COLON, *row, *col);
    }
    if (*src == '<' && src[1] && src[1] == '<') {
        *i += 2, *col += 2;
        return token_alloc("<<", TOKEN_TYPE_DOUBLE_LESSTHAN, *row, *col);
    }
    if (*src == '>' && src[1] && src[1] == '>') {
        *i += 2, *col += 2;
        return token_alloc(">>", TOKEN_TYPE_DOUBLE_GREATERTHAN, *row, *col);
    }
    if (*src == '*' && src[1] && src[1] == '*') {
        *i += 2, *col += 2;
        return token_alloc("**", TOKEN_TYPE_DOUBLE_ASTERISK, *row, *col);
    }
    if (*src == '|' && src[1] && src[1] == '>') {
        *i += 2, *col += 2;
        return token_alloc("|>", TOKEN_TYPE_PIPE_GREATERTHAN, *row, *col);
    }

    // 1 length symbols
    if (*src == '(') {
        *i += 1, *col += 1;
        return token_alloc("(", TOKEN_TYPE_LEFT_PARENTHESIS, *row, *col);
    }
    if (*src == ')') {
        *i += 1, *col += 1;
        return token_alloc(")", TOKEN_TYPE_RIGHT_PARENTHESIS, *row, *col);
    }
    if (*src == '{') {
        *i += 1, *col += 1;
        return token_alloc("{", TOKEN_TYPE_LEFT_CURLY_BRACKET, *row, *col);
    }
    if (*src == '}') {
        *i += 1, *col += 1;
        return token_alloc("}", TOKEN_TYPE_RIGHT_CURLY_BRACKET, *row, *col);
    }
    if (*src == '[') {
        *i += 1, *col += 1;
        return token_alloc("[", TOKEN_TYPE_LEFT_SQUARE_BRACKET, *row, *col);
    }
    if (*src == ']') {
        *i += 1, *col += 1;
        return token_alloc("[", TOKEN_TYPE_RIGHT_SQUARE_BRACKET, *row, *col);
    }
    if (*src == '#') {
        *i += 1, *col += 1;
        return token_alloc("#", TOKEN_TYPE_HASH, *row, *col);
    }
    if (*src == '.') {
        *i += 1, *col += 1;
        return token_alloc(".", TOKEN_TYPE_PERIOD, *row, *col);
    }
    if (*src == ';') {
        *i += 1, *col += 1;
        return token_alloc(";", TOKEN_TYPE_SEMICOLON, *row, *col);
    }
    if (*src == ',') {
        *i += 1, *col += 1;
        return token_alloc(",", TOKEN_TYPE_COMMA, *row, *col);
    }
    if (*src == '>') {
        *i += 1, *col += 1;
        return token_alloc(">", TOKEN_TYPE_GREATERTHAN, *row, *col);
    }
    if (*src == '<') {
        *i += 1, *col += 1;
        return token_alloc("<", TOKEN_TYPE_LESSTHAN, *row, *col);
    }
    if (*src == '=') {
        *i += 1, *col += 1;
        return token_alloc("=", TOKEN_TYPE_EQUALS, *row, *col);
    }
    if (*src == '&') {
        *i += 1, *col += 1;
        return token_alloc("&", TOKEN_TYPE_AMPERSAND, *row, *col);
    }
    if (*src == '*') {
        *i += 1, *col += 1;
        return token_alloc("*", TOKEN_TYPE_ASTERISK, *row, *col);
    }
    if (*src == '+') {
        *i += 1, *col += 1;
        return token_alloc("+", TOKEN_TYPE_PLUS, *row, *col);
    }
    if (*src == '-') {
        *i += 1, *col += 1;
        return token_alloc("-", TOKEN_TYPE_MINUS, *row, *col);
    }
    if (*src == '/') {
        *i += 1, *col += 1;
        return token_alloc("/", TOKEN_TYPE_FORWARD_SLASH, *row, *col);
    }
    if (*src == '|') {
        *i += 1, *col += 1;
        return token_alloc("|", TOKEN_TYPE_PIPE, *row, *col);
    }
    if (*src == '^') {
        *i += 1, *col += 1;
        return token_alloc("^", TOKEN_TYPE_CARET, *row, *col);
    }
    if (*src == '?') {
        *i += 1, *col += 1;
        return token_alloc("?", TOKEN_TYPE_QUESTION_MARK, *row, *col);
    }
    if (*src == '\\') {
        *i += 1, *col += 1;
        return token_alloc("\\", TOKEN_TYPE_BACKWARD_SLASH, *row, *col);
    }
    if (*src == '!') {
        *i += 1, *col += 1;
        return token_alloc("!", TOKEN_TYPE_BANG, *row, *col);
    }
    if (*src == '@') {
        *i += 1, *col += 1;
        return token_alloc("@", TOKEN_TYPE_AT, *row, *col);
    }
    if (*src == '$') {
        *i += 1, *col += 1;
        return token_alloc("$", TOKEN_TYPE_DOLLAR_SIGN, *row, *col);
    }
    if (*src == '%') {
        *i += 1, *col += 1;
        return token_alloc("%", TOKEN_TYPE_PERCENT, *row, *col);
    }
    if (*src == '`') {
        *i += 1, *col += 1;
        return token_alloc("`", TOKEN_TYPE_BACKTICK, *row, *col);
    }
    if (*src == '~') {
        *i += 1, *col += 1;
        return token_alloc("~", TOKEN_TYPE_TILDE, *row, *col);
    }
    if (*src == ':') {
        *i += 1, *col += 1;
        return token_alloc(":", TOKEN_TYPE_COLON, *row, *col);
    }

    fprintf(stderr, "error: unknown symbol: %c", *src);
    exit(1);
}

void lexer_append(lexer_t *lexer, token_t *tok) {
    if (!lexer->hd && !lexer->tl) {
        lexer->hd = tok;
        lexer->tl = lexer->hd;
    } else {
        token_t *tmp = lexer->tl;
        lexer->tl = tok;
        tmp->next = lexer->tl;
    }
}

int lexer_ok(const lexer_t *lexer) {
    return lexer->hd != NULL;
}

void lexer_discard(lexer_t *lexer) {
    lexer->hd = lexer->hd->next;
}

token_t *lexer_next(lexer_t *lexer) {
    token_t *hd = lexer->hd;
    lexer->hd = lexer->hd->next;
    return hd;
}

token_t *lexer_peek(lexer_t *lexer, int i) {
    token_t *it = lexer->hd;
    while (it && i >= 0) {
        if (i == 0)
            return it;
        ++it, --i;
    }
    return NULL;
}

void lexer_dump(lexer_t *lexer) {
    token_t *it = lexer->hd;
    while (it) {
        printf("{\n");
        printf("  lx: %s\n", it->lx);
        printf("  type: %d (%s)\n", (int)it->type, token_type_to_cstr(it->type));
        printf("  row: %zu\n", it->row);
        printf("  col: %zu\n", it->col);
        printf("}\n");
        it = it->next;
    }
}

// Returns the original position of `i` when first
// called. This way you can use subtraction to get
// the substring.
static size_t consume_while(const char *s, size_t *i, int (*pred)(int c)) {
    size_t n = *i, it = 0;
    while (s[it] && pred((int)s[it]))
        ++it;
    *i += it;
    return n;
}

static int not_eol(int c) {
    return !((char)c == '\n');
}

static int ident_ok(int c) {
    return isalnum((char)c) || (char)c == '_';
}

static int not_quote(int c) {
    return !((char)c == '"');
}

static int not_single_quote(int c) {
    return !((char)c == '\'');
}

lexer_t lexer_lex_src_code(const char *src, const char *fp) {
    (void)fp;

    lexer_t lexer = {
        .hd = NULL,
        .tl = NULL,
        .arena = arena_create(NULL),
    };

    size_t row = 1, col = 0, i = 0;

    while (src[i]) {
        char c = src[i];

        if (c == ' ' || c == '\t') {         /////// IGNORE ///////
            ++i, ++col;
        } else if (c == '\n' || c == '\r') { /////// NEWLINES ///////
            ++i, ++row, col = 0;
        } else if (c == '#') {               /////// COMMENTS ///////
            size_t start = consume_while(&src[i], &i, not_eol);
            size_t diff = i-start;
            col += diff;
        } else if (c == '"') {               /////// STRINGS ///////
            ++i, ++col; // skip past first quote
            size_t start = consume_while(&src[i], &i, not_quote);
            size_t diff = i-start;
            token_t *tok = token_alloc_from_substr(&src[start], diff,
                                                   TOKEN_TYPE_STRING_LITERAL,
                                                   row, col);
            lexer_append(&lexer, tok);
            col += diff+1; // +1 for ending "
            ++i;           // for ending "
        } else if (c == '\'') {              /////// CHARACTERS ///////
            ++i, ++col; // skip past first quote
            size_t start = consume_while(&src[i], &i, not_single_quote);
            size_t diff = i-start;
            token_t *tok = token_alloc_from_substr(&src[start], diff,
                                                   TOKEN_TYPE_CHARACTER_LITERAL,
                                                   row, col);
            lexer_append(&lexer, tok);
            col += diff+1; // +1 for ending "
            ++i;           // for ending "
        } else if (isalpha(c) || c == '_') { /////// IDENTIFIERS ///////
            size_t start = consume_while(&src[i], &i, ident_ok);
            size_t diff = i-start;

            token_t *tok = token_alloc_from_substr(&src[start], diff,
                                                   TOKEN_TYPE_IDENTIFIER,
                                                   row, col);

            if (keywords_is_keyword(tok->lx))
                tok->type = TOKEN_TYPE_KEYWORD;

            lexer_append(&lexer, tok);
            col += diff;
        } else if (isdigit(c)) {             /////// INTEGERS ///////
            size_t start = consume_while(&src[i], &i, isdigit);
            size_t diff = i-start;
            token_t *tok = token_alloc_from_substr(&src[start], diff,
                                                   TOKEN_TYPE_INTEGER_LITERAL,
                                                   row, col);
            lexer_append(&lexer, tok);
            col += diff;
        } else {                            /////// SYMBOLS ///////
            token_t *sym = handle_symbol(&src[i], &row, &col, &i);
            lexer_append(&lexer, sym);
        }
    }

    token_t *eof = token_alloc("EOF", TOKEN_TYPE_EOF, row, col);

    lexer_append(&lexer, eof);

    return lexer;
}
