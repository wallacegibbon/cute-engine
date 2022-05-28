#pragma once

#include "Node.h"
#include "Vendor.h"

namespace cute {

/// Represents an edge between two Nodes.
/// An Edge is defined by the value of its starting ("from") and ending ("to") Nodes.
/// Therefore two Edges are equivalent if they both start and end at the same (i.e. equivalent) Nodes.

class Edge {
public:
    Edge() {}
    Edge(const Node &from, const Node &to, int weight);

    Node from() const { return from_; }
    Node to() const { return to_; }
    int weight() const { return weight_; }

private:
    int weight_;
    Node from_;
    Node to_;
};

bool operator==(const Edge &lhs, const Edge &rhs);

} // namespace cute

namespace std {

template <>
struct hash<cute::Edge> {
    size_t operator()(const cute::Edge &edge) const {
        return hash<cute::Node>()(edge.from()) ^ hash<cute::Node>()(edge.to());
    }
};

} // namespace std
