#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "token.h"
#include "lexer.h"
#include "arena.h"

size_t
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

size_t
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

char *
find_multiline_comment_end(char *s, char *comment_end, size_t *row, size_t *col)
{
  size_t comment_end_len = strlen(comment_end);
  for (size_t i = 0; s[i]; ++i) {
    if (strncmp(s, comment_end, comment_end_len) == 0) {
      return s+i;
    }
    if (s[i] == '\n') {
      (*row)++;
      *col = 1;
    }
    else {
      (*col)++;
    }
  }
  return s;
}

int
is_newline(char c)
{
  return c == '\n';
}

int
is_quote(char c)
{
  return c == '"';
}

int
nisdigit(char c)
{
  return !isdigit(c);
}

int
nisvalid_ident(char c) {
  return !(c == '_' || isalnum(c));
}

// Code from:
//   chux - Reinstate Monica
//   https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
char *
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
  char *buffer = malloc(ulength + 1);

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
  if (tok) {
    lexer->hd = tok->next;
    lexer->len--;
  }
  return tok;
}

void
lexer_dump(struct lexer *lexer)
{
  struct token *tok;
  while ((tok = lexer_next(lexer))) {
    printf("lexeme: \"%s\", type: %s, row: %zu, col: %zu, fp: %s\n",
           tok->lexeme, tokentype_to_str(tok->type), tok->row, tok->col, tok->fp);
  }
}

int
is_keyword(char *s, size_t len, char **keywords, size_t keywords_len)
{
  for (size_t i = 0; i < keywords_len; ++i) {
    if (strncmp(s, keywords[i], len) == 0) {
      return 1;
    }
  }
  return 0;
}

int
try_comment(char *src, char *comment)
{
  if (strncmp(src, comment, strlen(comment)) == 0) {
    return find_comment_end(src);
  }

  return 0;
}

/* char * */
/* try_multiline_comment(char *src, char *comment_start, char *comment_end, size_t *row, size_t *col) */
/* { */
/*   assert(0 && "try_multiline_comment: unimplemented"); */

/*   if (strncmp(src, comment_start, strlen(comment_start)) == 0) { */
/*     return find_multiline_comment_end(src, comment_end, row, col); */
/*   } */

/*   return 0; */
/* } */

#define SYMTIDX(c)                              \
  ((c == '(') ? 0 :                             \
   (c == ')') ? 1 :                             \
   (c == '[') ? 2 :                             \
   (c == ']') ? 3 :                             \
   (c == '{') ? 4 :                             \
   (c == '}') ? 5 :                             \
   (c == '#') ? 6 :                             \
   (c == '.') ? 7 :                             \
   (c == ';') ? 8 :                             \
   (c == ',') ? 9 :                             \
   (c == '>') ? 10 :                            \
   (c == '<') ? 11 :                            \
   (c == '=') ? 12 :                            \
   (c == '&') ? 13 :                            \
   (c == '*') ? 14 :                            \
   (c == '+') ? 15 :                            \
   (c == '-') ? 16 :                            \
   (c == '/') ? 17 :                            \
   (c == '|') ? 18 :                            \
   (c == '^') ? 19 :                            \
   (c == '?') ? 20 :                            \
   (c == '\\') ? 21 :                           \
   (c == '!') ? 22 :                            \
   (c == '@') ? 23 :                            \
   (c == '$') ? 24 :                            \
   (c == '%') ? 25 :                            \
   (c == '`') ? 26 :                            \
   (c == '~') ? 27 :                            \
   (c == ':') ? 28 : -1)

void
assert_symtbl_inorder(int *symtbl)
{
  for (size_t i = 0; i < TOKENTYPE_SYM_LEN-1; ++i) {
    if (symtbl[i] != symtbl[i+1]-1) {
      fprintf(stderr, "ERR: symtbl out of order. left = %s, right = %s\n",
              tokentype_to_str(symtbl[i]), tokentype_to_str(symtbl[i+1]));
      exit(EXIT_FAILURE);
    }
  }
}

void
assert_symtidx_inorder(void)
{
  char order[] = {
    '(', ')', '[', ']', '{',
    '}', '#', '.', ';', ',',
    '>', '<', '=', '&', '*',
    '+', '-', '/', '|', '^',
    '?', '\\', '!', '@', '$',
    '%', '`', '~', ':',
  };

  assert(TOKENTYPE_SYM_LEN == sizeof(order)/sizeof(*order));

  for (size_t i = 0; i < TOKENTYPE_SYM_LEN; ++i) {
    assert(SYMTIDX(order[i]) == (int)i);
  }
}

struct lexer
lex_file(char *filepath, char **keywords, size_t keywords_len, char *comment)
{
  int symtbl[TOKENTYPE_SYM_LEN] = {
    TOKENTYPE_LPAREN,
    TOKENTYPE_RPAREN,
    TOKENTYPE_LBRACKET,
    TOKENTYPE_RBRACKET,
    TOKENTYPE_LBRACE,
    TOKENTYPE_RBRACE,
    TOKENTYPE_HASH,
    TOKENTYPE_PERIOD,
    TOKENTYPE_SEMICOLON,
    TOKENTYPE_COMMA,
    TOKENTYPE_GREATERTHAN,
    TOKENTYPE_LESSTHAN,
    TOKENTYPE_EQUALS,
    TOKENTYPE_AMPERSAND,
    TOKENTYPE_ASTERISK,
    TOKENTYPE_PLUS,
    TOKENTYPE_MINUS,
    TOKENTYPE_FORWARDSLASH,
    TOKENTYPE_PIPE,
    TOKENTYPE_CARET,
    TOKENTYPE_QUESTIONMARK,
    TOKENTYPE_BACKWARDSLASH,
    TOKENTYPE_BANG,
    TOKENTYPE_AT,
    TOKENTYPE_DOLLARSIGN,
    TOKENTYPE_PERCENT,
    TOKENTYPE_BACKTICK,
    TOKENTYPE_TILDE,
    TOKENTYPE_COLON,
  };

  assert_symtbl_inorder(symtbl);
  assert_symtidx_inorder();

  char *src = file_to_str(filepath);
  struct lexer lexer = (struct lexer) {
    .hd = NULL,
    .tl = NULL,
    .len = 0,
    .arena = arena_create(32768),
  };

  size_t i, row, col;
  for (i = 0, row = 1, col = 1; src[i]; ++i) {
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

    switch (c) {
    case '\r':
    case '\n':
      ++row;
      col = 1;
      break;
    case '\t':
    case ' ':
      ++col;
      break;
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '#':
    case '.':
    case ',':
    case ';':
    case '>':
    case '<':
    case '=':
    case '&':
    case '*':
    case '+':
    case '-':
    case '/':
    case '|':
    case '^':
    case '?':
    case '\\':
    case '!':
    case '@':
    case '$':
    case '%':
    case '`':
    case '~':
    case ':': {
      tok = token_alloc(&lexer, lexeme, 1, symtbl[SYMTIDX(c)], row, col, filepath);
      lexer_append(&lexer, tok);
      ++col;
    } break;
    case '"': {
      size_t strlit_len = consume_until(lexeme+1, is_quote);
      tok = token_alloc(&lexer, lexeme+1, strlit_len, TOKENTYPE_STRLIT, row, col, filepath);
      lexer_append(&lexer, tok);
      i += 1+strlit_len;
      col += 1+strlit_len+1;
    } break;
    case '\'':
      ++lexeme;
      tok = token_alloc(&lexer, lexeme, 1, TOKENTYPE_CHARLIT, row, col, filepath);
      lexer_append(&lexer, tok);
      i += 2;
      ++col;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      size_t intlit_len = consume_until(lexeme, nisdigit);
      tok = token_alloc(&lexer, lexeme, intlit_len, TOKENTYPE_INTLIT, row, col, filepath);
      lexer_append(&lexer, tok);
      i += intlit_len-1;
      col += intlit_len;
    } break;
    default: { // Idents, keywords
      size_t ident_len = consume_until(lexeme, nisvalid_ident);
      enum token_type type = is_keyword(lexeme, ident_len, keywords, keywords_len) ? TOKENTYPE_KEYWORD : TOKENTYPE_IDENT;
      tok = token_alloc(&lexer, lexeme, ident_len, type, row, col, filepath);
      lexer_append(&lexer, tok);
      i += ident_len-1;
      col += ident_len;
    } break;
    }
  }

  lexer_append(&lexer, token_alloc(&lexer, "EOF", 3, TOKENTYPE_EOF, row, col, filepath));

  free(src);
  return lexer;
}

void
lexer_free(struct lexer *lexer)
{
  arena_free(lexer->arena);
  lexer->len = 0;
}
