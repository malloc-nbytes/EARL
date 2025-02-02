#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#include "mem/arena.h"

/// @brief A safer malloc() wrapper
/// @param bytes The number of bytes to allocate
/// @param arena (optional) The arena to use for the allocator
/// @param allocator (optional) The allocator to use
/// @return A pointer to the allocated memory
void *mem_s_malloc(size_t bytes, arena_t *arena, void *(allocator)(arena_t *, size_t));

#endif // MEM_H
