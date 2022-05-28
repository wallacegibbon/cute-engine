#include "Edge.h"

using namespace cute;

/// Constructs an Edge from the specfieid Nodes and of the specified weight.
Edge::Edge(const Node &from, const Node &to, int weight) : from_(from), to_(to), weight_(weight) {
    assert(!(from == to));
}

/// Returns true if the two edges have the same starting and ending Nodes, _regardeless of their weights_.
bool cute::operator==(const Edge &lhs, const Edge &rhs) { return (lhs.from() == rhs.from()) && (lhs.to() == rhs.to()); }
