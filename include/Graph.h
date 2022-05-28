#pragma once

#include "Edge.h"
#include "Node.h"
#include "Vendor.h"

namespace cute {

class Tree;

/// A Graph is a set of Nodes and Edges.
class Graph {
public:
    /// Please ensure that all of the Nodes of the Edges are actually Nodes in the set of Nodes.
    /// In other words, please make sure that the set of Nodes and Edges being passed in actually represent a graph.
    /// This constructor does not do any checking.
    Graph(const std::unordered_set<Node> &nodes, const std::unordered_set<Edge> &edges)
            : nodes_(nodes), edges_(edges) {}

    /// constructs a Graph with no Nodes or Edges (empty Graph).
    Graph() {}

    std::unordered_set<Node> nodes() const { return nodes_; }
    std::unordered_set<Edge> edges() const { return edges_; }
    std::vector<Edge> outgoing_edges(const Node &from) const;
    std::vector<Edge> incoming_edges(const Node &to) const;

    /// all ADJACENT nodes this node can go to
    std::vector<Node> outgoing_nodes(const Node &from) const;

    /// all ADJACENT nodes that can come to this node
    std::vector<Node> incoming_nodes(const Node &to) const;

    bool contains(const Node &node) const;
    bool contains(const Edge &edge) const;
    std::vector<Node> shortest_path(const Node &from, const Node &to) const;
    Tree spt(const Node &source) const;

    void add_node(const Node &node);
    void add_edge(const Node &from, const Node &to, int weight);

private:
    std::unordered_set<Node> nodes_;
    std::unordered_set<Edge> edges_;

    /// helper attributes for dijkstras
    mutable std::unordered_set<Node> picked_nodes_;
    mutable std::unordered_set<Node> unpicked_nodes_;
    mutable std::unordered_set<Edge> picked_edges_;
    mutable std::unordered_set<Edge> unpicked_edges_;
    mutable std::unordered_map<Node, int> node_weight_;
    mutable std::unordered_map<Node, Edge> updated_edge_;

    /// helper functions for dijkstras
    void pick(const Node &node) const;
    void pick(const Edge &edge) const;
    bool picked(const Node &node) const;
    bool picked(const Edge &edge) const;
    void set_weight(const Node &node, int weight) const;
    int weight(const Node &node) const;
    Edge edge(const Node &from, const Node &to) const;
    void unpick_all() const;
    void initialize_node_weights(const Node &source) const;
    bool has_unpicked_node() const;
    Node lightest_unpicked_node() const;
    void pick_conneted_edge(const Node &node) const;
    std::vector<Node> unpicked_neighbors(const Node &node) const;
    void update_neighbor_weights(const Node &node) const;

    /// helper attributes for A*
    mutable std::unordered_set<Node> open_nodes_;
    mutable std::unordered_set<Node> closed_nodes_;
    mutable std::unordered_map<Node, Node> node_to_parent_;
    mutable std::unordered_map<Node, int> node_to_F_cost_;
    mutable std::unordered_map<Node, int> node_to_G_cost_;
    mutable std::unordered_map<Node, int> node_to_H_cost_;

    /// helper functions for A*
    Node node_in_open_with_lowest_F_cost() const;
    void calculate_costs(Node from, Node to) const;
};

} // namespace cute
