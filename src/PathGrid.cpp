#include "PathGrid.h"
#include "Tree.h"

using namespace cute;

PathGrid::PathGrid(int num_cols, int num_rows) : num_cols_(num_cols), num_rows_(num_rows) {
    assert((num_cols >= 0) && (num_rows >= 0));

    /// populate with nodes
    for (size_t i = 0; i < num_cols; i++) {
        std::vector<Node> col_of_nodes;
        for (size_t j = 0; j < num_rows; j++) {
            Node node(i, j);
            col_of_nodes.push_back(node);
        }
        nodes_.push_back(col_of_nodes);
    }

    /// make all nodes unfilled
    for (Node node : nodes()) {
        filled_[node] = false;
    }
}

/// A PathGrid maintains a mapping of Node:bool determining which Nodes are filled. This member function
/// will mark the specified node as filled.
void PathGrid::fill(const Node &node) { filled_[node] = true; }

void PathGrid::fill(int x, int y) { fill(Node(x, y)); }

void PathGrid::fill() {
    for (Node node : nodes()) {
        fill(node);
    }
}

void PathGrid::unfill(const Node &node) { filled_[node] = false; }

void PathGrid::unfill(int x, int y) { unfill(Node(x, y)); }

void PathGrid::unfill() {
    for (Node node : nodes()) {
        unfill(node);
    }
}

/// Fills/unfills Nodes based on the values of a 2d int vector.
///
/// A value of 0 means that the specfied Node should be unfilled. Any other value
/// means that it should be filled. Please ensure that the 2d vector is the same
/// size as the PathGrid. If it is shorter, the remaining Nodes will be unfilled.
/// If it is longer, you will get an out of range error.=
void PathGrid::set_filling(const std::vector<std::vector<int>> &vec) {
    for (int y = 0, n = nodes_[0].size(); y < n; y++) {
        for (int x = 0, p = nodes_.size(); x < p; x++) {
            if (vec[y][x] == 0) {
                unfill(x, y);
            } else {
                fill(x, y);
            }
        }
    }
}

/// Sets the filling _starting_ at the specified location to be the same filling
/// as that of the specified PathGrid.
void PathGrid::set_filling(const PathGrid &path_grid, const Node &pos) {
    /// approach:
    /// -traverse through Nodes of new PathGrid
    /// -set shadowed Node to be the same fillness

    for (Node node : path_grid.nodes()) {
        Node shadowed(node.x() + pos.x(), node.y() + pos.y());
        if (path_grid.filled(node)) {
            fill(shadowed);
        } else {
            unfill(shadowed);
        }
    }
}

/// Adds the specified PathGrid to this PathGrid at the specified positino.
///
/// The resulting PathGrid can be more filled but not less filled.
void PathGrid::add_path_grid(const PathGrid &path_grid, const Node &pos) {
    /// approach
    /// -traverse through Nodes of new path
    /// -if Node is filled, make sure shadowed is filled too

    for (Node node : path_grid.nodes()) {
        Node shadowed(node.x() + pos.x(), node.y() + pos.y());
        /// if node is filled and shadowed node is not
        if (path_grid.filled(node)) {
            fill(shadowed);
        }
    }
}

bool PathGrid::filled(const Node &node) const {
    assert(contains(node));
    return filled_.find(node)->second;
}

bool PathGrid::filled(int x, int y) const { return filled(Node(x, y)); }

/// Returns a vector of all the adjacent unfilled neighboring Nodes of the specified node.
///
/// This represents all the diretions in which one could move from the specfied Node.
std::vector<Node> PathGrid::unfilled_neighbors(const Node &node) const {
    std::vector<Node> neighbors;
    /// up
    if (node.y() > 0) {
        Node n(node.x(), node.y() - 1);
        if (!filled(n)) {
            neighbors.push_back(n);
        }
    }
    /// down
    if (node.y() < num_rows_ - 1) {
        Node n(node.x(), node.y() + 1);
        if (!filled(n)) {
            neighbors.push_back(n);
        }
    }
    /// left
    if (node.x() > 0) {
        Node n(node.x() - 1, node.y());
        if (!filled(n)) {
            neighbors.push_back(n);
        }
    }
    /// right
    if (node.x() < num_cols_ - 1) {
        Node n(node.x() + 1, node.y());
        if (!filled(n)) {
            neighbors.push_back(n);
        }
    }
    return neighbors;
}

/// Returns a Graph representation of the PathGrid.
///
/// More specifically, returns a Graph containing all the *unfilled* Nodes and the Edges between them.
/// The start and end node will be included in the graph even if they are filled,
/// that is why they must be passed to this function.
Graph PathGrid::to_graph(const Node &start, const Node &end) const {
    Graph g;

    /// add all unfilled nodes (add start/end Nodes even if they are filled)
    for (Node node : nodes()) {
        if (!filled(node) || node == start || node == end) {
            g.add_node(node);
        }
    }

    /// add all edges between unfilled nodes
    for (Node node : g.nodes()) {
        for (Node neighbor : unfilled_neighbors(node)) {
            g.add_edge(node, neighbor, 1);
            g.add_edge(neighbor, node, 1);
        }
    }

    return g;
}

std::vector<Node> PathGrid::shortest_path(const Node &from, const Node &to) const {
    return to_graph(from, to).shortest_path(from, to);
}

std::vector<Node> PathGrid::column(int i) const { return nodes(Node(i, 0), Node(i, num_rows_ - 1)); }

std::vector<Node> PathGrid::row(int i) const { return nodes(Node(0, i), Node(num_cols_ - 1, i)); }

std::vector<Node> PathGrid::nodes(const Node &top_left, const Node &bottom_right) const {
    std::vector<Node> nodes_in_region;
    for (int x = top_left.x(), n = bottom_right.x(); x <= n; x++) {
        for (int y = top_left.y(), p = bottom_right.y(); y <= p; y++) {
            if (contains(Node(x, y))) {
                nodes_in_region.push_back(Node(x, y));
            }
        }
    }
    return nodes_in_region;
}

std::vector<Node> PathGrid::nodes() const { return nodes(Node(0, 0), Node(num_cols_ - 1, num_rows_ - 1)); }

bool PathGrid::contains(const Node &node) const { return node.x() < num_cols_ && node.y() < num_rows_; }
