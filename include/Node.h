#pragma once

#include "Vendor.h"

namespace cute {

class Graph;

class Node {
public:
    Node(int x, int y) : x_(x), y_(y) {}
    Node() {}

    int x() const { return x_; }
    int y() const { return y_; }

    void set_x(int x) { x_ = x; }
    void set_y(int y) { y_ = y; }

private:
    int x_ = 0;
    int y_ = 0;
};

// non member comparison operators
bool operator==(const cute::Node &lhs, const cute::Node &rhs);
bool operator!=(const cute::Node &lhs, const cute::Node &rhs);

} // namespace cute

// make node hashable
namespace std {
template <>
struct hash<cute::Node> {
    size_t operator()(const cute::Node &node) const { return hash<int>()(node.x()) + hash<int>()(node.y()); }
};
} // namespace std
