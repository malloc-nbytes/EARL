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
