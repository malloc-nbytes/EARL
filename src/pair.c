#include <assert.h>
#include <string.h>

#include "pair.h"

void
pair_make_unique(struct pair *pair, void *fst, void *snd)
{
  assert(pair->fst && pair->snd);
  (void)memcpy(pair->fst, fst, pair->fst_stride);
  (void)memcpy(pair->snd, snd, pair->snd_stride);
}

void *
pair_fst(struct pair *p)
{
  assert(p->fst);
  return p->fst;
}

void *
pair_snd(struct pair *p)
{
  assert(p->snd);
  return p->snd;
}

void
pair_free(struct pair *p)
{
  free(p->fst);
  free(p->snd);
}
