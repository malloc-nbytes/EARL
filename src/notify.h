#ifndef NOTIFY_H
#define NOTIFY_H

#include <stdio.h>
#include <stdlib.h>

enum NOTIFY_ERR_TYPE {
  NOTIFY_ERR_FATAL = 0,
  NOTIFY_ERR_SYNTAX,
  NOTIFY_ERR_UNKNOWN,
  NOTIFY_ERR_USAGE,
};

#define NOTIFY_ERR(etype, msg)                         \
  do {                                                 \
    fprintf(stderr, "EARL: [" #etype "] " msg "\n");   \
    exit(EXIT_FAILURE);                                \
  } while (0)

#define NOTIFY_ERRARGS(etype, msg, ...)                               \
  do {                                                                \
    fprintf(stderr, "EARL: [" #etype "] " msg "\n", ##__VA_ARGS__);   \
    exit(EXIT_FAILURE);                                               \
  } while (0)

#endif // NOTIFY_H
