#ifndef ERR_H
#define ERR_H

enum class ErrType {
  Syntax,
  Runtime,
  Internal,
  Fatal,
  Redeclared,
  Undeclared,
};

#define ERR_WARGS(errtype, msg, ...) \
  do { \
    fprintf(stderr, "%s: ", __func__); \
    fprintf(stderr, msg, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    std::exit(1); \
  } while (0)

#define ERR(errtype, msg) \
  do { \
    fprintf(stderr, "%s: ", __func__); \
    fprintf(stderr, msg); \
    fprintf(stderr, "\n"); \
    std::exit(1); \
  } while (0)

#endif // ERR_H
