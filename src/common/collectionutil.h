/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

namespace reone {

template <class T>
inline bool isOutOfRange(const std::vector<T> &arr, int index) {
    return index < 0 || index >= static_cast<int>(arr.size());
}

template <class Src, class Dest>
inline std::vector<Dest> transform(const std::vector<Src> &source, const std::function<Dest(const Src &)> &fn) {
    std::vector<Dest> result;
    for (auto &item : source) {
        result.push_back(fn(item));
    }
    return std::move(result);
}

template <class Src, class Dest>
inline std::map<Src, Dest> associate(const std::vector<Src> &source, const std::function<Dest(const Src &)> &fn) {
    std::map<Src, Dest> result;
    for (auto &item : source) {
        result.insert(make_pair(item, fn(item)));
    }
    return move(result);
}

template <class Src, class K, class V>
inline std::map<K, V> associate(
    const std::vector<Src> &source,
    const std::function<K(const Src &)> &keyFn,
    const std::function<V(const Src &)> &valueFn) {
    std::map<K, V> result;
    for (auto &item : source) {
        result.insert(make_pair(keyFn(item), valueFn(item)));
    }
    return move(result);
}

template <class K, class V>
inline V getFromLookupOrNull(const std::map<K, V> &lookup, K key) {
    auto maybeValue = lookup.find(key);
    return maybeValue != lookup.end() ? maybeValue->second : nullptr;
}

template <class K, class V>
inline V getFromLookupOrElse(const std::map<K, V> &lookup, K key, V defaultValue) {
    auto maybeValue = lookup.find(key);
    return maybeValue != lookup.end() ? maybeValue->second : defaultValue;
}

template <class K, class V>
inline V getFromLookupOrElse(const std::map<K, V> &lookup, K key, std::function<std::string()> fn) {
    auto maybeValue = lookup.find(key);
    return maybeValue != lookup.end() ? maybeValue->second : fn();
}

template <class K, class V>
inline V getFromLookupOrNull(const std::unordered_map<K, V> &lookup, K key) {
    auto maybeValue = lookup.find(key);
    return maybeValue != lookup.end() ? maybeValue->second : nullptr;
}

template <class K, class V>
inline V getFromLookupOrElse(const std::unordered_map<K, V> &lookup, K key, V defaultValue) {
    auto maybeValue = lookup.find(key);
    return maybeValue != lookup.end() ? maybeValue->second : defaultValue;
}

template <class K, class V>
inline V getFromLookupOrElse(const std::unordered_map<K, V> &lookup, K key, std::function<std::string()> fn) {
    auto maybeValue = lookup.find(key);
    return maybeValue != lookup.end() ? maybeValue->second : fn();
}

template <class K, class V>
inline std::vector<V> mapToValues(const std::unordered_map<K, V> &map) {
    std::vector<V> values;
    for (auto &pair : map) {
        values.push_back(pair.second);
    }
    return move(values);
}

template <class K, class V>
inline std::vector<std::pair<K, V>> mapToEntries(const std::unordered_map<K, V> &map) {
    std::vector<std::pair<K, V>> entries;
    for (auto &pair : map) {
        entries.push_back(pair);
    }
    return move(entries);
}

} // namespace reone
