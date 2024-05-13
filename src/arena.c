#include <stdlib.h>

#include "arena.h"

struct arena {
  uint8_t *mem;
  size_t len;
  size_t cap;
};

uint8_t *
arena_alloc(struct arena *arena, size_t bytes)
{
  if (arena->len+bytes > arena->cap) {
    arena->cap *= 2;
    arena->mem = realloc(arena->mem, arena->cap);
  }
  uint8_t *mem = &arena->mem[arena->len];
  arena->len += bytes;
  return mem;
}

void
arena_free(struct arena *arena)
{
  free(arena->mem);
  free(arena);
}

struct arena *
arena_create(size_t cap)
{
  struct arena *arena = malloc(sizeof(struct arena));
  arena->mem = malloc(cap);
  arena->cap = cap;
  arena->len = 0;
  return arena;
}
