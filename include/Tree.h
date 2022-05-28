#pragma once

#include "Graph.h"
#include "Node.h"
#include "Vendor.h"

namespace cute {

/// A Tree is a Graph with no cycles.
class Tree {
public:
    Tree(const Node &root);
    Tree(const Graph &graph, const Node &root);

    std::vector<Node> path_to(const Node &node) const;
    std::unordered_set<Node> nodes() const;
    std::unordered_set<Edge> edges() const;

    void add_child(const Node &to, const Node &child, int weight);

private:
    Graph graph_;
    Node root_;

    mutable std::unordered_set<Node> visited_nodes_;

    void visit(const Node &node) const;
    bool is_visited(const Node &node) const;
    bool has_unvisited_child(const Node &node) const;
    Node an_unvisited_child(const Node &node) const;
    std::vector<Node> dfs(const Node &node, const Node &target, std::vector<Node> path) const;
};

} // namespace cute
