#ifndef COMMON_H
#define COMMON_H

#include <map>
#include <set>
#include <vector>

template <typename Container>
auto flatten(const Container& outer) {
    typename Container::value_type flat;
    auto it = flat.begin();
    for (const auto& inner: outer) {
        it = flat.insert(it, inner.cbegin(), inner.cend());
    }
    return flat;
}

template <typename Container>
Container unique(const Container& c) {
    std::set<typename Container::value_type> s{c.cbegin(), c.cend()};
    return {s.cbegin(), s.cend()};
}

template <typename Key, typename Value>
std::vector<Key> map_keys_to_vector(const std::map<Key, Value>& m) {
    std::vector<Key> v;
    v.reserve(m.size());
    std::transform(
            m.cbegin(), m.cend(),
            std::back_inserter(v),
            [](const auto& pair) { return pair.first; }
            );
    return v;
}

template <typename Key, typename Value>
std::vector<Value> map_values_to_vector(const std::map<Key, Value>& m) {
    std::vector<Value> v;
    v.reserve(m.size());
    std::transform(
            m.cbegin(), m.cend(),
            std::back_inserter(v),
            [](const auto& pair) { return pair.second; }
            );
    return v;
}

#endif
