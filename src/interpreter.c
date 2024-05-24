#include <stdio.h>
#include <string.h>

#include "interpreter.h"
#include "hashtbl.h"
#include "vector.h"
#include "token.h"
#include "ast.h"

struct var {
  struct token *id;
  struct token *type;
  void *value;
};

struct vector(hashtbl(char **, struct var *)) scope;

static unsigned
__symtbl_hash(void *k, size_t bytes)
{
  (void)bytes;
  return strlen(*(char **)k);
}

static int
__symtbl_keycompar(void *k1, void *k2)
{
  return utils_streq(*(char **)k1, *(char **)k2);
}

static void
scope_push(void)
{
  struct hashtbl ht = hashtbl_create2(char **, struct var *, __symtbl_hash, __symtbl_keycompar);
  vector_append(&scope, &ht);
}

int
interpret(struct program *program)
{
  NOOP(__symtbl_hash);
  NOOP(__symtbl_keycompar);
  NOOP(scope_push);
  NOOP(program);

  scope = vector_create2(struct hashtbl(char **, struct var *));

  return 0;
}
