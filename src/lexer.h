#ifndef LEXER_H
#define LEXER_H

#include "arena.h"
#include "token.h"

struct lexer {
  struct token *hd;
  struct token *tl;
  size_t len;
  struct arena *arena;
};

void lexer_append(struct lexer *lexer, struct token *tok);
struct token *lexer_next(struct lexer *lexer);
void lexer_dump(struct lexer *lexer);
struct lexer lex_file(char *filepath, char **keywords, size_t keywords_len, char *comment);
void lexer_free(struct lexer *lexer);

#endif // LEXER_H
