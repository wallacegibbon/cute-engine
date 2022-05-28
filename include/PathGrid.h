#pragma once

#include "Graph.h"
#include "Node.h"
#include "Vendor.h"

namespace cute {

/// Represents a grid of Nodes, each of which can either be filled or unfilled.
/// The (x,y) value of the top left node is (0,0). The (x,y) value of the bottom right node is (width,height).

class PathGrid {
public:
    PathGrid() : num_cols_(0), num_rows_(0) {}
    PathGrid(int num_cols, int num_rows);

    /// make compiler generate default copy ctor
    PathGrid(const PathGrid &copy) = default;

    bool filled(const Node &node) const;
    bool filled(int x, int y) const;

    std::vector<Node> unfilled_neighbors(const Node &node) const;

    std::vector<Node> shortest_path(const Node &from, const Node &to) const;

    std::vector<Node> nodes(const Node &top_left, const Node &bottom_right) const;
    std::vector<Node> nodes() const;

    std::vector<Node> column(int i) const;
    std::vector<Node> row(int i) const;

    bool contains(const Node &node) const;

    int num_cols() const { return num_cols_; }
    int num_rows() const { return num_rows_; }

    void fill(const Node &node);
    void fill(int x, int y);
    void fill();
    void unfill(const Node &node);
    void unfill(int x, int y);
    void unfill();
    void set_filling(const std::vector<std::vector<int>> &vec);
    void set_filling(const PathGrid &path_grid, const Node &pos);
    void add_path_grid(const PathGrid &path_grid, const Node &pos);

private:
    std::vector<std::vector<Node>> nodes_;
    std::unordered_map<Node, bool> filled_;
    int num_cols_;
    int num_rows_;

    Graph to_graph(const Node &start, const Node &end) const;
};

} // namespace cute
