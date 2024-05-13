#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

struct program parse(struct lexer lexer);

#endif // PARSER_H
