#ifndef ERR_H
#define ERR_H

#include <stdio.h>
#include <stdlib.h>

enum ERR_TYPE {
  ERR_FATAL = 0,
  ERR_SYNTAX,
  ERR_UNKNOWN,
  ERR_USAGE,
};

#define ERR(etype, msg)                              \
  do {                                               \
    fprintf(stderr, "EARL: [" #etype "] " msg "\n");   \
    exit(EXIT_FAILURE);                              \
  } while (0)

#define ERRARGS(etype, msg, ...)                                         \
  do {                                                                   \
    fprintf(stderr, "EARL: [" #etype "] " msg "\n", ##__VA_ARGS__);   \
    exit(EXIT_FAILURE);                                                  \
  } while (0)


#endif // ERR_H
