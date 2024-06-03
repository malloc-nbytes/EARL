#ifndef UNIQUE_SCOPE_H
#define UNIQUE_SCOPE_H

#include <memory>
#include <vector>
#include <unordered_map>

template <typename K, typename V> struct UniqueScope {
    std::vector<std::unordered_map<K, std::unique_ptr<V>>> m_map;

    inline UniqueScope() {
        m_map.emplace_back();
    }

    inline ~UniqueScope() = default;

    // Disable copying
    UniqueScope(const UniqueScope&) = delete;
    UniqueScope& operator=(const UniqueScope&) = delete;

    // Move constructor and move assignment operator
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
