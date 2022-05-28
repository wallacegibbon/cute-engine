#pragma once

#include "Vendor.h"

namespace stl_helper {

/// @name find(container, item)
/// Finds an item in a container.
/// Returns an iterator to the item if found, otherwise returns the end iterator.
/// If the container is a map (or unordered map) 'item' should be a key.
/// In that case, if the key is found, you will get an iterator to the key-value pair, otherwise, you will get the end
/// iterator.
/// @note The most efficient search algorithm available for the container is used.
///@{
///
/// Generic find() overload that works on any container. Complexity is linear.
template <typename ContainerType, typename ItemType>
auto find(const ContainerType &container, const ItemType &item) {
    return std::find(std::begin(container), std::end(container), item);
}

/// find() overload for set, uses binary search tree, thus complexity is logarithmic.
template <typename ItemType>
auto find(const std::set<ItemType> &container, const ItemType &item) {
    return container.find(item);
}

/// find() overload for unordered set, uses hash table, thus complexity is constant.
template <typename ItemType>
auto find(const std::unordered_set<ItemType> &container, const ItemType &item) {
    return container.find(item);
}

/// find() overload for map, uses binary search tree, thus complexity is logarithmic.
template <typename KeyType, typename ValueType>
auto find(const std::map<KeyType, ValueType> &container, const KeyType &item) {
    return container.find(item);
}

/// find() overload for unordered map, uses hash table, thus complexity is constant.
template <typename KeyType, typename ValueType>
auto find(const std::unordered_map<KeyType, ValueType> &container, const KeyType &item) {
    return container.find(item);
}
///@}

template <typename ItemType>
int find_index(const std::vector<ItemType> &container, const ItemType &value) {
    int index = std::find(container.begin(), container.end(), value) - container.begin();
    if (index == container.size()) {
        return -1;
    } else {
        return index;
    }
}

/// Returns true if the specified container contains the specified item.
/// If the container is a map (or unordered map), 'item' should be a key.
/// @note The most efficient search algorithm available for the container is used.
template <typename ContainerType, typename ItemType>
bool contains(const ContainerType &container, const ItemType &item) {
    return find(container, item) != std::end(container);
}

/// @name contains_all(container,items)
/// Returns true if the specified container contains *all* the specified items.
/// If the container is a map (or unordered map), the items should be keys.
///@{
/// contains_all() overload for all types except initializer lists
template <typename ContainerToCheckType, typename ContainerOfItemsType>
bool contains_all(const ContainerToCheckType &container, const ContainerOfItemsType &items) {
    for (const auto &item : items) {
        if (!contains(container, item)) {
            return false;
        }
    }
    return true;
}

/// contains_all() overload for initializer lists
template <typename ContainerToCheckType, typename ItemType>
bool contains_all(const ContainerToCheckType &container, const std::initializer_list<ItemType> &items) {
    for (const auto &item : items) {
        if (!contains(container, item)) {
            return false;
        }
    }
    return true;
}
///@}

/// @name contains_any(container,items)
/// Returns true if the specified container contains *any* of the specified items.
/// If the container is a map (or unordered map), the items should be keys.
///@{
/// contains_any() overload for all types except initializer lists
template <typename ContainerToCheckType, typename ContainerOfItemsType>
bool contains_any(const ContainerToCheckType &container, const ContainerOfItemsType &items) {
    for (const auto &item : items) {
        if (contains(container, item)) {
            return true;
        }
    }
    return false;
}

/// contains_any() overload for initializer lists
template <typename ContainerToCheckType, typename ItemType>
bool contains_any(const ContainerToCheckType &container, const std::initializer_list<ItemType> &items) {
    for (const auto &item : items) {
        if (contains(container, item)) {
            return true;
        }
    }
    return false;
}
///@}

/// @name remove(from_container, item)
/// Removes the specified item from the container (if it exists).
/// @note The most efficient removal algorithm available for the container is used.
///@{
///
/// remove() Overload that works for any container that the "erase and remove" idiom works for.
/// Complexity is linear.
template <typename ContainerType, typename ItemType>
void remove(ContainerType &from_container, const ItemType &item) {
    auto iter = std::remove(std::begin(from_container), std::end(from_container), item);
    from_container.erase(iter, std::end(from_container));
}

/// remove() overload for set, complexity is logarithmic.
template <typename ItemType>
void remove(std::set<ItemType> &from_container, const ItemType &item) {
    from_container.erase(item);
}

/// remove() overload for unordred set, complexity is constant.
template <typename ItemType>
void remove(std::unordered_set<ItemType> &from_container, const ItemType &item) {
    from_container.erase(item);
}
/// remove() overload for map, complexity is logarithmic.
template <typename KeyType, typename ValueType>
void remove(std::map<KeyType, ValueType> &from_container, const KeyType &item) {
    from_container.erase(item);
}

/// remove() overload for unordered map, complexity is constant.
template <typename KeyType, typename ValueType>
void remove(std::unordered_map<KeyType, ValueType> &from_container, const KeyType &item) {
    from_container.erase(item);
}
///@}

/// Adds an item to the end of a container.
/// @note Uses the most efficient insertion operation available for the container.
template <typename ContainerType, typename ItemType>
void add(ContainerType &container, const ItemType &item) {
    container.insert(std::end(container), item);
}

/// Adds a key and value to a map (or unordered map).
template <typename MapType, typename KeyType, typename ValueType>
void add(MapType &in_map, const KeyType &key, const ValueType &value) {
    in_map[key] = value;
}

/// @name count(inContianer, item)
/// Returns the number of copies of an item that are in a container.
/// @note Uses the most efficient search operation available for the container.
///@{
///
/// Generic count() overload that works for any container.
template <typename ContainerType, typename ItemType>
size_t count(const ContainerType &container, const ItemType &item) {
    return std::count(std::begin(container), std::end(container), item);
}

/// count() overload for set, complexity is logarithmic.
template <typename ItemType>
size_t count(const std::set<ItemType> &container, const ItemType &item) {
    return container.count(item);
}

/// count() overload for unordered set, complexity is constant.
template <typename ItemType>
size_t count(const std::unordered_set<ItemType> &container, const ItemType &item) {
    return container.count(item);
}

/// count() overload for map, complexity is logarithmic.
template <typename KeyType, typename ValueType>
size_t count(const std::map<KeyType, ValueType> &container, const KeyType &item) {
    return container.count(item);
}

/// count() overload for unordered map, complexity is constant.
template <typename KeyType, typename ValueType>
size_t count(const std::unordered_map<KeyType, ValueType> &container, const KeyType &item) {
    return container.count(item);
}
///@}

inline bool contain_digit(const std::string &str) {
    return std::find_if(str.begin(), str.end(), [](char c) { return isdigit(c); }) != str.end();
}

} // namespace stl_helper
