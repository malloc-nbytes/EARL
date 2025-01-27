#ifndef CSTR_MAP_H
#define CSTR_MAP_H

#include <stdint.h>
#include <stddef.h>

#define s_umap_t(ty) s_umap_t

/// @brief Buckets that the u_map holds
typedef struct s_umap_bucket {
    /// @brief The key
    char *key;

    /// @brief The value
    uint8_t *value;

    /// @brief The next bucket
    struct s_umap_bucket *next;
} s_umap_bucket_t;

/// @brief A (very) basic unordered_map<char *, T> implementation
typedef struct {
    /// @brief The table of buckets
    s_umap_bucket_t **tbl;

    /// @brief The number of buckets the tbl holds
    size_t sz;

    /// @brief The capacity of tbl
    size_t cap;

    /// @brief The hashing function
    unsigned long (*hash)(const char *);

    /// @brief The destructor for values
    /// @note Can be NULL
    void (*destroy_value)(uint8_t *x);
} s_umap_t;

/// @brief Create a new s_umap
/// @param hash The hashing function
/// @param destroy_value (Optional) a destructor for the values
/// @return A new s_umap
s_umap_t s_umap_create(unsigned long (*hash)(const char *), void (*destroy_value)(uint8_t *));

/// @brief Remove an element from the map
/// @param map The map to use
/// @param key The key to remove
void s_umap_remove(s_umap_t *map, const char *key);

/// @brief Insert an element into the map
/// @param map The map to use
/// @param key The key to use
/// @param value The value to use
void s_umap_insert(s_umap_t *map, const char *key, uint8_t *value);

/// @brief Debug print all elements contained in the map
/// @param map The map to use
/// @param display The function to use to print the values
void s_umap_dump(s_umap_t *map, void (*display)(const uint8_t *));

/// @brief Destroy the map
/// @param map The map to use
void s_umap_destroy(s_umap_t *map);

/// @brief Get the value from the associated key in the map
/// @param map The map to use
/// @param key The key to use
/// @return The value that the key is associated with
void *s_umap_get(s_umap_t *map, const char *key);

/// @brief Check to see if the map contains an element
/// @param map The map to use
/// @param key The key to use
/// @return 1 if it has it, and 0 if otherwise
int s_umap_contains(s_umap_t *map, const char *key);

#endif // CSTR_MAP_H
