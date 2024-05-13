#ifndef PAIR_H
#define PAIR_H

#include <stdint.h>
#include <stddef.h>

struct pair {
  uint8_t *fst;
  size_t fst_stride;
  uint8_t *snd;
  size_t snd_stride;
};

#define pair_create(t1, t2)                     \
  (struct pair) {                               \
    .fst = malloc(sizeof(t1)),                  \
    .fst_stride = sizeof(t1),                   \
    .snd = malloc(sizeof(t2)),                  \
    .snd_stride = sizeof(t2),                   \
  }

#define pair(t1, t2) pair

#define pair_unsafe_fst(p, cast) (*((cast*)(p).fst))
#define pair_unsafe_snd(p, cast) (*((cast*)(p).snd))

void pair_make_unique(struct pair *pair, void *fst, void *snd);
void *pair_fst(struct pair *p);
void *pair_snd(struct pair *p);
void pair_free(struct pair *p);

#endif // PAIR_H
