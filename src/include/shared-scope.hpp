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

#ifndef SHARED_SCOPE_H
#define SHARED_SCOPE_H

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>

/**
 * A scope structure that holds `shared_ptr<V>` as the
 * value.
 */

template <typename K, typename V> struct SharedScope {
    std::vector<std::unordered_map<K, std::shared_ptr<V>>> m_map;

    inline SharedScope() {
        m_map.emplace_back();
    }

    inline ~SharedScope() = default;

    SharedScope(const SharedScope&) = delete;
    SharedScope& operator=(const SharedScope&) = delete;

    SharedScope(SharedScope&&) noexcept = default;
    SharedScope& operator=(SharedScope&&) noexcept = default;

    inline SharedScope copy(void) {
        SharedScope copy;
        for (size_t i = 0; i < m_map.size(); ++i) {
            auto el = m_map.at(i);
            for (auto it = el.begin(); it != el.end(); ++it) {
                copy.add(it->first, it->second);
            }
            if (i != m_map.size())
                copy.push();
        }
        return copy;
    }

    inline size_t size(void) const {
        size_t res = 0;
        for (auto &el : m_map) {
            res += el.size();
        }
        return res;
    }

    inline void push(void) {
        m_map.emplace_back();
    }

    inline void pop(void) {
        m_map.pop_back();
    }

    inline void add(K key, std::shared_ptr<V> value) {
        m_map.back().emplace(key, value);
    }

    inline bool contains(const K key) const {
        for (auto &map : m_map) {
            if (map.find(key) != map.end())
                return true;
        }
        return false;
    }

    inline std::shared_ptr<V> get(K key) {
        for (auto it = m_map.rbegin(); it != m_map.rend(); ++it) {
            auto &map = *it;
            if (map.find(key) != map.end()) {
                return map.at(key);
            }
        }
        return nullptr;
    }

    inline void remove(K key) {
        for (auto it = m_map.rbegin(); it != m_map.rend(); ++it) {
            auto &map = *it;
            auto map_it = map.find(key);
            if (map_it != map.end()) {
                map.erase(map_it);
                return;
            }
        }
    }

    inline std::vector<std::shared_ptr<V>> extract_tovec(void) {
        std::vector<std::shared_ptr<V>> vec = {};
        for (const auto &map : m_map) {
            for (const auto &pair : map) {
                vec.push_back(pair.second);
            }
        }
        return vec;
    }

    inline void clear(void) {
        m_map.clear();
    }

    inline void debug_dump(void) const {
        int i = 1;
        for (const auto &map : m_map) {
            std::cout << "  Scope level: " << i << std::endl;
            std::cout << "    ";
            for (const auto &pair : map) {
                std::cout << pair.first << " ";
            }
            std::cout << std::endl;
            ++i;
        }
    }

};

#endif // SHARED_H
