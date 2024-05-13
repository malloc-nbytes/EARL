#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "arena.h"
#include "token.h"
#include "lexer.h"

char *
tokentype_to_str(enum token_type type)
{
  switch (type) {
  case TOKENTYPE_LPAREN:
    return "LPAREN";
  case TOKENTYPE_RPAREN:
    return "RPAREN";
  case TOKENTYPE_LBRACKET:
    return "LBRACKET";
  case TOKENTYPE_RBRACKET:
    return "RBRACKET";
  case TOKENTYPE_LBRACE:
    return "LBRACE";
  case TOKENTYPE_RBRACE:
    return "RBRACE";
  case TOKENTYPE_HASH:
    return "HASH";
  case TOKENTYPE_PERIOD:
    return "PERIOD";
  case TOKENTYPE_COMMA:
    return "COMMA";
  case TOKENTYPE_SEMICOLON:
    return "SEMICOLON";
  case TOKENTYPE_GREATERTHAN:
    return "GREATERTHAN";
  case TOKENTYPE_LESSTHAN:
    return "LESSTHAN";
  case TOKENTYPE_EQUALS:
    return "EQUALS";
  case TOKENTYPE_AMPERSAND:
    return "AMPERSAND";
  case TOKENTYPE_ASTERISK:
    return "ASTERISK";
  case TOKENTYPE_PLUS:
    return "PLUS";
  case TOKENTYPE_MINUS:
    return "MINUS";
  case TOKENTYPE_FORWARDSLASH:
    return "FORWARDSLASH";
  case TOKENTYPE_PIPE:
    return "PIPE";
  case TOKENTYPE_CARET:
    return "CARET";
  case TOKENTYPE_QUESTIONMARK:
    return "QUESTIONMARK";
  case TOKENTYPE_BACKWARDSLASH:
    return "BACKWARDSLASH";
  case TOKENTYPE_BANG:
    return "BANG";
  case TOKENTYPE_AT:
    return "AT";
  case TOKENTYPE_DOLLARSIGN:
    return "DOLLARSIGN";
  case TOKENTYPE_PERCENT:
    return "PERCENT";
  case TOKENTYPE_BACKTICK:
    return "BACKTICK";
  case TOKENTYPE_TILDE:
    return "TILDE";
  case TOKENTYPE_COLON:
    return "COLON";
  case TOKENTYPE_SYM_LEN:
    assert(0 && "should not use TOKENTYPE_SYM_LEN");
    return NULL;
  case TOKENTYPE_EOF:
    return "EOF";
  case TOKENTYPE_INTLIT:
    return "INTLIT";
  case TOKENTYPE_STRLIT:
    return "STRLIT";
  case TOKENTYPE_CHARLIT:
    return "CHARLIT";
  case TOKENTYPE_IDENT:
    return "IDENT";
  case TOKENTYPE_KEYWORD:
    return "KEYWORD";
  default:
    assert(0 && "unknown type");
    return NULL;
  }
  return NULL;
}

struct token *
token_alloc(struct lexer *lexer,
                   char *start, size_t len,
                   enum token_type type,
                   size_t row, size_t col, char *fp)
{
  struct token *tok = (struct token *)arena_alloc(lexer->arena, sizeof(struct token));

  char *lexeme = (char *)arena_alloc(lexer->arena, len+1);
  (void)strncpy(lexeme, start, len);
  lexeme[len] = '\0';

  tok->lexeme = lexeme;
  tok->type = type;
  tok->row = row;
  tok->col = col;
  tok->fp = fp;
  tok->next = NULL;

  return tok;
}
