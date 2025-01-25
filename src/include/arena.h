/** file */

// MIT License

// Copyright (c) 2025 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>

#define ARENA_DEFAULT_SZ 2056

/// @brief The arena allocator
struct arena {
        /// @brief The memory that is used to allocate from
        uint8_t *mem;

        /// @brief The length of the memory
        size_t len;

        /// @brief The capacity of the memory
        size_t cap;
};

/// @brief Create a default arena
/// @param bytes The number of bytes to reserve
/// @note The bytes parameter is optional, will
///       use ARENA_DEFAULT_SZ if it is NULL
struct arena arena_create(size_t *bytes);

/// @brief Initialize an arena with an existing one
/// @param arena The arena to use
/// @param bytes The number of bytes to use
/// @note The bytes parameter is optional, will
///       use ARENA_DEFAULT_SZ if it is NULL
void arena_create_from(struct arena *arena, size_t *bytes);

/// @brief Request memory from the arena
/// @param arena The arena to use
/// @param bytes The number of bytes to allocate
uint8_t *arena_malloc(struct arena *arena, size_t bytes);

/// @brief Free the arena
/// @param arena The arena to use
/// @note Will call free() on arena itself
void arena_free(struct arena *arena);

#endif // ARENA_H
