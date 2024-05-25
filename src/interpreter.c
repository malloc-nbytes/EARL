#include <stdio.h>
#include <string.h>

#include "interpreter.h"
#include "hashtbl.h"
#include "notify.h"
#include "common.h"
#include "vector.h"
#include "utils.h"
#include "token.h"
#include "ast.h"

struct var {
  struct token *id;
  struct token *type;
  void *value;
};

struct vector(hashtbl(char **, struct var **)) scope;

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
  struct hashtbl ht = hashtbl_create2(char **, struct var **, __symtbl_hash, __symtbl_keycompar);
  vector_append(&scope, &ht);
}

static int
var_in_scope(char *id)
{
  for (size_t i = 0; i < scope.len; ++i) {
    struct hashtbl *ht = (struct hashtbl *)vector_at(&scope, i);
    struct token *tok;
    if ((tok = (struct token *)hashtbl_get(ht, id)) != NULL) {
      return 1;
    }
  }
  return 0;
}

static size_t
earltype_to_ctype_asbytes(struct token *ty)
{
  if (utils_streq(ty->lexeme, COMMON_TY_INT32)) {
    return sizeof(int);
  }
  else if (utils_streq(ty->lexeme, COMMON_TY_STR)) {
    return sizeof(char *);
  }
  NOTIFY_ERRARGS(NOTIFY_ERR_SYNTAX, "earltype_to_ctype_asbytes: invalid EARL type: %s", ty->lexeme);
}

static struct var *
var_alloc(struct token *id, struct token *type, void *value)
{
  size_t tybytes = earltype_to_ctype_asbytes(type);

  struct var *var = utils_safe_malloc(sizeof(struct var));
  var->id = id;
  var->type = type;
  var->value = utils_safe_malloc(tybytes);

  if (value != NULL) {
    (void)memcpy(var->value, value, tybytes);
  }

  return var;
}

static void
add_var_to_scope(struct token *id, struct token *ty, void *value)
{
  struct hashtbl *ht = (struct hashtbl *)vector_at(&scope, scope.len-1);
  struct var *var = var_alloc(id, ty, value);

  assert(ht != NULL);
  assert(var != NULL);

  printf("HERE1\n");
  hashtbl_insert(ht, id->lexeme, var);
  printf("HERE2\n");
}

static void
eval_stmt_let(struct stmt_let *stmt)
{
  if (var_in_scope(stmt->id->lexeme)) {
    NOTIFY_ERRARGS(NOTIFY_ERR_SYNTAX,
                   "eval_stmt_let: variable %s is already defined", stmt->id->lexeme);
  }
  add_var_to_scope(stmt->id, stmt->type, NULL);
}

static void
eval_stmt(struct stmt *stmt)
{
  switch (stmt->type) {
  case STMT_TYPE_LET:
    eval_stmt_let(stmt->stmt.let);
    break;
  case STMT_TYPE_DEF:
    UNIMPLEMENTED("eval_stmt: STMT_TYPE_DEF");
  default:
    NOTIFY_ERRARGS(NOTIFY_ERR_SYNTAX, "eval_stmt: unknown stmt type: %d", stmt->type);
  }
}

int
interpret(struct program *program)
{
  scope_push();

  scope = vector_create2(struct hashtbl(char **, struct var *));

  for (size_t i = 0; i < (program->stmts).len; ++i) {
    struct stmt *stmt = *(struct stmt **)vector_at(&program->stmts, i);
    eval_stmt(stmt);
  }

  return 0;
}
