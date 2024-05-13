#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>

struct arena;

uint8_t *arena_alloc(struct arena *arena, size_t bytes);
void arena_free(struct arena *arena);
struct arena *arena_create(size_t cap);

#endif // ARENA_H
