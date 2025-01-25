#ifndef CSTR_MAP_H
#define CSTR_MAP_H

#include <stdint.h>
#include <stddef.h>

#define s_umap(t1, t2) s_umap

/// @brief Buckets that the u_map holds
struct s_umap_bucket {
        /// @brief The key
        char *key;

        /// @brief The value
        uint8_t *value;

        /// @brief The next bucket
        struct s_umap_bucket *next;
};

/// @brief A (very) basic unordered_map implementation
struct s_umap {
        /// @brief The table of buckets
        struct s_umap_bucket **tbl;

        /// @brief The number of buckets the tbl holds
        size_t sz;

        /// @brief The capacity of tbl
        size_t cap;

        /// @brief The hashing function
        unsigned long (*hash)(const char *);

        /// @brief The destructor for values
        /// @note Can be NULL
        void (*destroy_value)(uint8_t *x);
};

/// @brief Create a new s_umap
/// @param hash The hashing function
/// @param destroy_value (Optional) a destructor for the values
/// @return A new s_umap
struct s_umap s_umap_create(unsigned long (*hash)(const char *), void (*destroy_value)(uint8_t *));

/// @brief Remove an element from the map
/// @param map The map to use
/// @param key The key to remove
void s_umap_remove(struct s_umap *map, const char *key);

/// @brief Insert an element into the map
/// @param map The map to use
/// @param key The key to use
/// @param value The value to use
void s_umap_insert(struct s_umap *map, const char *key, uint8_t *value);

/// @brief Debug print all elements contained in the map
/// @param map The map to use
/// @param display The function to use to print the values
void s_umap_dump(struct s_umap *map, void (*display)(const uint8_t *));

/// @brief Destroy the map
/// @param map The map to use
void s_umap_destroy(struct s_umap *map);

/// @brief Get the value from the associated key in the map
/// @param map The map to use
/// @param key The key to use
/// @return The value that the key is associated with
void *s_umap_get(struct s_umap *map, const char *key);

/// @brief Check to see if the map contains an element
/// @param map The map to use
/// @param key The key to use
/// @return 1 if it has it, and 0 if otherwise
int s_umap_contains(struct s_umap *map, const char *key);

#endif // CSTR_MAP_H
