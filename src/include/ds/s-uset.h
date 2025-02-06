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

#ifndef S_USET_H
#define S_USET_H

#include <stddef.h>

/// @brief Nodes that the u_set holds
typedef struct s_uset_node {
    /// @brief The value
    char *value;

    /// @brief The next node
    struct s_uset_node *next;
} s_uset_node_t;

/// @brief A (very) basic unordered_set<char *> implementation
typedef struct {
    /// @brief The table of the nodes
    s_uset_node_t **tbl;

    /// @brief The number of buckets the tbl holds
    size_t sz;

    /// @brief The capacity of tbl
    size_t cap;

    /// @brief The hashing function
    unsigned long (*hash)(const char *);
} s_uset_t;

/// @brief Create a new set
/// @param hash The hashing function
/// @return The new set
s_uset_t s_uset_create(unsigned long (*hash)(const char *));

/// @brief Destroy a set
/// @param set The set to destroy
void s_uset_destroy(s_uset_t *set);

/// @brief Insert a value into the set
/// @param set The set to use
/// @param value The value to insert
void s_uset_insert(s_uset_t *set, const char *value);

/// @brief Remove a value from the set
/// @param set The set to use
/// @param value The value to remove
void s_uset_remove(s_uset_t *set, const char *value);

/// @brief Check if the set contains a value
/// @param set The set to use
/// @param value The value to check for
/// @return 1 if found, 0 if otherwise
int s_uset_contains(s_uset_t *set, const char *value);

#endif // S_USET_H
