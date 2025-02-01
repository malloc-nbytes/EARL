#ifndef LOG_H
#define LOG_H

#define LOG_KILL(msg, ...)                      \
    do {                                        \
        fprintf(stderr, msg, __VA_ARGS__);      \
        exit(1);                                \
    } while (0)

#endif // LOG_H
