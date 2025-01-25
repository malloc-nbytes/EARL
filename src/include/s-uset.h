#ifndef S_USET_H
#define S_USET_H

#include <stddef.h>

struct s_uset_node {
        char *value;
        struct s_uset_node *next;
};

struct s_uset {
        struct s_uset_node **tbl;
        size_t sz;
        size_t cap;
        unsigned long (*hash)(const char *);
};

struct s_uset s_uset_create(unsigned long (*hash)(const char *));
void s_uset_destroy(struct s_uset *set);
void s_uset_insert(struct s_uset *set, const char *value);
void s_uset_remove(struct s_uset *set, const char *value);
int s_uset_contains(struct s_uset *set, const char *value);

#endif // S_USET_H
