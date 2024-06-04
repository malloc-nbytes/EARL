#ifndef SCOPE_H
#define SCOPE_H

#include <unordered_map>
#include <vector>

template <typename K, typename V> struct Scope {
    std::vector<std::unordered_map<K, V>> m_map;

    Scope() {
        m_map.emplace_back();
    }

    ~Scope() = default;

    inline void push(void) {
        m_map.emplace_back();
    }

    inline void pop(void) {
        m_map.pop_back();
    }

    inline bool contains(K key) {
       for (auto &map : m_map) {
            if (map.find(key) != map.end())
                return true;
        }
        return false;
    }

    inline void add(K key, V value) {
        m_map.back().emplace(key, value);
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
