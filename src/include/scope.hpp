/** @file */

// MIT License

// Copyright (c) 2023 malloc-nbytes

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

#ifndef SCOPE_H
#define SCOPE_H

#include <iostream>
#include <unordered_map>
#include <vector>

/// @brief A generic scope structure
template <typename K, typename V> struct Scope {

    /// @brief The map from `K` -> `V` that represents
    /// the scope.
    std::vector<std::unordered_map<K, V>> m_map;

    Scope() {
        m_map.emplace_back();
    }

    ~Scope() = default;

    inline void clear(void) {
        if (!m_map.empty()) {
            m_map.back().clear();
        }
    }

    /// @brief Pushes a new scope context
    inline void push(void) {
        m_map.emplace_back();
    }

    /// @brief Pops a scope context
    inline void pop(void) {
        m_map.pop_back();
    }

    /// @brief Check to see if `key` is a part
    /// of any of the scope contexts.
    /// @param key The key to find
    /// @returns True if present, false if otherwise
    inline bool contains(const K key) {
       for (auto &map : m_map) {
            if (map.find(key) != map.end()) {
                return true;
            }
        }
        return false;
    }

    /// @brief Add a key-value to the scope
    /// @param key The key
    /// @param value The value
    inline void add(K key, V value) {
        m_map.back().emplace(key, value);
    }

    /// @brief Get the value associated with `key` in
    /// any of the scope contexts.
    /// @param key The key associated with the value needed
    inline V *get(K key) {
        for (auto it = m_map.rbegin(); it != m_map.rend(); ++it) {
            auto &map = *it;
            if (map.find(key) != map.end()) {
                return &map.at(key);
            }
        }
        return nullptr;
    }

    /// @brief Remove a key from the scope
    /// @param key The key to remove
    inline void remove(K key) {
        for (auto it = m_map.rbegin(); it != m_map.rend(); ++it) {
            auto &map = *it;
            auto mapIt = map.find(key);
            if (mapIt != map.end()) {
                map.erase(mapIt);
                return;
            }
        }
    }

    inline void debug_dump(void) const {
        int i = 1;
        for (const auto &map : m_map) {
            std::cout << "Scope level: " << i << std::endl;
            std::cout << "    ";
            for (const auto &pair : map) {
                std::cout << pair.first << " ";
            }
            std::cout << std::endl;
            ++i;
        }
    }
};

#endif // SCOPE_H
