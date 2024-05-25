// MIT License

// Copyright (c) 2023 malloc-nbytes

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

#include <string>
#include <unordered_map>

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "notify.h"
#include "token.h"
#include "lexer.h"
#include "utils.h"
#include "arena.h"

static size_t
consume_until(char *s, int (*predicate)(char))
{
  size_t i;
  int skip = 0;
  for (i = 0; s[i]; ++i) {
    if (!skip && predicate(s[i])) {
      return i;
    }
    if (s[i] == '\\') {
      skip = 1;
    }
    else {
      skip = 0;
    }
  }
  return i;
}

static size_t
find_comment_end(char *s)
{
  size_t i;
  for (i = 0; s[i]; ++i) {
    if (s[i] == '\n') {
      return i;
    }
  }
  return i;
}

static int
is_newline(char c)
{
  return c == '\n';
}

static int
is_quote(char c)
{
  return c == '"';
}

static int
nisdigit(char c)
{
  return !isdigit(c);
}

static int
nisvalid_ident(char c) {
  return !(c == '_' || isalnum(c));
}

// Code from:
//   chux - Reinstate Monica
//   https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
static char *
file_to_str(char *filepath) {
  FILE *f = fopen(filepath, "rb");

  if (f == NULL || fseek(f, 0, SEEK_END)) {
    return NULL;
  }

  long length = ftell(f);
  rewind(f);
  if (length == -1 || (unsigned long) length >= SIZE_MAX) {
    return NULL;
  }

  size_t ulength = (size_t)length;
  char *buffer = static_cast<char *>(utils_safe_malloc(ulength+1));

  if (buffer == NULL || fread(buffer, 1, ulength, f) != ulength) {
    free(buffer);
    return NULL;
  }
  buffer[ulength] = '\0';

  return buffer;
}

void
lexer_append(struct lexer *lexer, struct token *tok)
{
  if (!lexer->hd) {
    lexer->hd = tok;
    lexer->tl = tok;
  } else {
    lexer->tl->next = tok;
    lexer->tl = tok;
  }
  ++lexer->len;
}

struct token *
lexer_next(struct lexer *lexer)
{
  struct token *tok = lexer->hd;
  if (!tok) {
    NOTIFY_ERR(NOTIFY_ERR_NOTOKENS, "lexer_next: no more tokens");
  }
  lexer->hd = tok->next;
  lexer->len--;
  return tok;
}

void
lexer_discard(struct lexer *lexer)
{
  if (!lexer->hd) {
    NOTIFY_ERR(NOTIFY_ERR_NOTOKENS, "lexer_discard: no more tokens");
  }
  lexer->hd = lexer->hd->next;
  lexer->len--;
}

void
lexer_dump(struct lexer *lexer)
{
  struct token *it = lexer->hd;
  while (it) {
    printf("lexeme: \"%s\", type: %s, row: %zu, col: %zu, fp: %s\n",
           it->lexeme, tokentype_to_str(it->type), it->row, it->col, it->fp);
    it = it->next;
  }
}

static int
is_keyword(char *s, size_t len, char **keywords, size_t keywords_len)
{
  // TODO: possible stack overflow.
  char real[256];
  memset(real, '\0', len);
  memcpy(real, s, len);

  for (size_t i = 0; i < keywords_len; ++i) {
    if (utils_streq(real, keywords[i])) {
      return 1;
    }
  }

  return 0;
}

static int
issym(char c)
{
  return !isalnum(c) && c != '_';
}

int
try_comment(char *src, char *comment)
{
  if (strncmp(src, comment, strlen(comment)) == 0) {
    return find_comment_end(src);
  }

  return 0;
}

struct token *
lexer_peek(struct lexer *lexer, size_t n)
{
  struct token *it = lexer->hd;

  while (it && n != 0) {
    it = it->next;
  }

  return it;
}

void
lexer_free(struct lexer *lexer)
{
  arena_free(lexer->arena);
  lexer->len = 0;
}

static int
__keycompar(void *k1, void *k2)
{
  return strcmp(*(char **)k1, *(char **)k2);
}

static unsigned
__hashfunc(void *x, size_t bytes)
{
  (void)bytes;
  return strlen(*(char **)x);
}

struct lexer
lex_file(char *filepath, char **keywords, size_t keywords_len, char *comment)
{
  char *src = file_to_str(filepath);

  struct lexer lexer = (struct lexer) {
    .hd = NULL,
    .tl = NULL,
    .arena = arena_create(32768),
    .len = 0,
  };

  std::unordered_map<std::string, token_type> ht;

  ht["("] = TOKENTYPE_LPAREN;
  ht[")"] = TOKENTYPE_RPAREN;
  ht["["] = TOKENTYPE_LBRACKET;
  ht["]"] = TOKENTYPE_RBRACKET;
  ht["{"] = TOKENTYPE_LBRACE;
  ht["}"] = TOKENTYPE_RBRACE;
  ht["#"] = TOKENTYPE_HASH;
  ht["."] = TOKENTYPE_PERIOD;
  ht[";"] = TOKENTYPE_SEMICOLON;
  ht[","] = TOKENTYPE_COMMA;
  ht[">"] = TOKENTYPE_GREATERTHAN;
  ht["<"] = TOKENTYPE_LESSTHAN;
  ht["="] = TOKENTYPE_EQUALS;
  ht["&"] = TOKENTYPE_AMPERSAND;
  ht["*"] = TOKENTYPE_ASTERISK;
  ht["+"] = TOKENTYPE_PLUS;
  ht["-"] = TOKENTYPE_MINUS;
  ht["/"] = TOKENTYPE_FORWARDSLASH;
  ht["|"] = TOKENTYPE_PIPE;
  ht["^"] = TOKENTYPE_CARET;
  ht["?"] = TOKENTYPE_QUESTIONMARK;
  ht["\\"] = TOKENTYPE_BACKWARDSLASH;
  ht["!"] = TOKENTYPE_BANG;
  ht["@"] = TOKENTYPE_AT;
  ht["$"] = TOKENTYPE_DOLLARSIGN;
  ht["%"] = TOKENTYPE_PERCENT;
  ht["`"] = TOKENTYPE_BACKTICK;
  ht["~"] = TOKENTYPE_TILDE;
  ht[":"] = TOKENTYPE_COLON;
  ht["&&"] = TOKENTYPE_DOUBLE_AMPERSAND;
  ht["||"] = TOKENTYPE_DOUBLE_PIPE;
  ht[">="] = TOKENTYPE_GREATERTHAN_EQUALS;
  ht["<="] = TOKENTYPE_LESSTHAN_EQUALS;
  ht["=="] = TOKENTYPE_DOUBLE_EQUALS;
  ht["!="] = TOKENTYPE_BANG_EQUALS;
  ht["+="] = TOKENTYPE_PLUS_EQUALS;
  ht["-="] = TOKENTYPE_MINUS_EQUALS;
  ht["*="] = TOKENTYPE_ASTERISK_EQUALS;
  ht["/="] = TOKENTYPE_FORWARDSLASH_EQUALS;
  ht["%="] = TOKENTYPE_PERCENT_EQUALS;

  const size_t bufcap = 256;
  char *buf = (char *)malloc(bufcap); // For search symbols in `ht`.
  size_t i = 0, row = 1, col = 1;
  while (src[i]) {
    char c = src[i];
    struct token *tok = NULL;
    char *lexeme = src+i;

    // Single line comment
    if (c == comment[0]) {
      size_t comment_len;
      if ((comment_len = try_comment(lexeme, comment)) >= strlen(comment)) {
        i += comment_len;
        col = 1;
        ++row;
        continue;
      }
      else {
        // Reset
        lexeme = src+i;
      }
    }

    // Newlines
    if (c == '\r' || c == '\n') {
      ++row;
      col = 1;
      ++i;
    }

    // Tabs/empty spaces
    else if (c == '\t' || c == ' ') {
      ++col;
      ++i;
    }

    // String literal
    else if (c == '"') {
      size_t strlit_len = consume_until(lexeme+1, is_quote);
      tok = token_alloc(&lexer, lexeme+1, strlit_len, TOKENTYPE_STRLIT, row, col, filepath);
      lexer_append(&lexer, tok);
      i += 1+strlit_len+1;
      col += 1+strlit_len+1;
    }

    // Char literal
    else if (c == '\'') {
      ++lexeme;
      tok = token_alloc(&lexer, lexeme, 1, TOKENTYPE_CHARLIT, row, col, filepath);
      lexer_append(&lexer, tok);
      i += 3;
      ++col;
    }

    // Keywords/Identifiers
    else if (isalpha(c) || c == '_') {
      size_t ident_len = consume_until(lexeme, nisvalid_ident);
      enum token_type type = is_keyword(lexeme, ident_len, keywords, keywords_len) ? TOKENTYPE_KEYWORD : TOKENTYPE_IDENT;
      tok = token_alloc(&lexer, lexeme, ident_len, type, row, col, filepath);
      lexer_append(&lexer, tok);
      i += ident_len;
      col += ident_len;
    }

    // Numbers
    else if (isdigit(c)) {
      size_t intlit_len = consume_until(lexeme, nisdigit);
      tok = token_alloc(&lexer, lexeme, intlit_len, TOKENTYPE_INTLIT, row, col, filepath);
      lexer_append(&lexer, tok);
      i += intlit_len;
      col += intlit_len;
    }

    // Symbols
    else {
      size_t buflen = 0;

      memset(buf, '\0', bufcap);

      for (size_t j = 0; src[i+j] && issym(src[i+j]); ++j) {
        if (src[i+j] != ' ' && src[i+j] != '\t' && src[i+j] != '\n' && src[i+j] != '\t') {
          buf[buflen++] = src[i+j];
        }
      }

      enum token_type *value = NULL;
      while (buflen > 0) {
        // if ((value = (enum token_type *)hashtbl_get(&ht, &buf)) != NULL) {
        //   break;
        // }
        buf[--buflen] = '\0';
      }

      if (!value) {
        NOTIFY_ERR(NOTIFY_ERR_FATAL, "lex_file: `value` from `ht` is NULL");
      }

      tok = token_alloc(&lexer, buf, buflen, *value, row, col, filepath);
      lexer_append(&lexer, tok);
      col += buflen;
      i += buflen;
    }
  }

  lexer_append(&lexer, token_alloc(&lexer, "EOF", 3, TOKENTYPE_EOF, row, col, filepath));

  free(buf);
  free(src);
  return lexer;
}
