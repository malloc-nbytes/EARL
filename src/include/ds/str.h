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

#ifndef STR_H
#define STR_H

#include <stddef.h>

/// @brief A basic String data structure
typedef struct {
    /// @brief The actual characters
    char *data;

    /// @brief The length of data
    size_t len;

    /// @brief The capacity of data
    size_t cap;
} str_t;

/// @brief Append a character to a string
/// @param s The string to append to
/// @param c The character to append
void str_append(str_t *s, char c);

/// @brief Concatenate a cstr
/// @param s The string to concat to
/// @param other The cstr to concat
void str_concat_from_cstr(str_t *s, const char *other);

/// @brief Allocate a string
/// @param from (Optional) fill the string with these characters
/// @return A newly heap allocated string
str_t *str_alloc(const char *from);

/// @brief Create a string on the stack
/// @param from (Optional) fill the string with these characters
/// @return A newly stack allocated string
str_t str_create(const char *from);

/// @brief Get the length of a string
/// @param s The string to get the length from
/// @return The length
size_t str_len(const str_t *const s);

/// @brief Clear a string
/// @param s The string to clear
void str_clear(str_t *s);

#endif // STR_H
