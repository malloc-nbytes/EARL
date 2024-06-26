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

#ifndef UNIQUE_SCOPE_H
#define UNIQUE_SCOPE_H

#include <memory>
#include <vector>
#include <unordered_map>

/**
 * A scope structure that holds `unique_ptr<V>` as the
 * value.
 */

template <typename K, typename V> struct UniqueScope {
    std::vector<std::unordered_map<K, std::unique_ptr<V>>> m_map;

    inline UniqueScope() {
        m_map.emplace_back();
    }

    inline ~UniqueScope() = default;

    UniqueScope(const UniqueScope&) = delete;
    UniqueScope& operator=(const UniqueScope&) = delete;

    UniqueScope(UniqueScope&&) noexcept = default;
    UniqueScope& operator=(UniqueScope&&) noexcept = default;

    inline void push(void) {
        m_map.emplace_back();
    }

    inline void pop(void) {
        m_map.pop_back();
    }

    inline void add(K key, std::unique_ptr<V> value) {
        m_map.back().emplace(key, std::move(value));
    }

    inline bool contains(K key) {
        for (auto &map : m_map) {
            if (map.find(key) != map.end())
                return true;
        }
        return false;
    }

    inline V *get(K key) {
        for (auto it = m_map.rbegin(); it != m_map.rend(); ++it) {
            auto &map = *it;
            if (map.find(key) != map.end()) {
                return map.at(key).get();
            }
        }
        return nullptr;
    }
};

#endif // SCOPE_H
