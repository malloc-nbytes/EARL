#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

#define FILEPATH "../input.txt"

int
main(void)
{
  char *keywords[] = {
    "def",
    "return",
    "int"
  };

  size_t keywords_len = sizeof(keywords)/sizeof(*keywords);

  char *comment = "#";

  struct lexer lexer = lex_file(FILEPATH, keywords, keywords_len, comment);

  lexer_dump(&lexer);

  return 0;
}
