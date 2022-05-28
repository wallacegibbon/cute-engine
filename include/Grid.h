#pragma once

#include "Vendor.h"

class QPointF;
class QRectF;

namespace cute {

class Node;

/// Represents a region of 2d space that is divided into a grid of cells.
/// You can do things such finding the cell at a specified point, finding the position of a specified cell,
/// getting a bounding box of a cell or some cells in a region, etc...
///
/// Here are a few things you can do to a Grid:
/// To get the top left position of a specified cell, use cell_to_point().
/// To get the cell at the specified point, use point_to_cell();
/// Functions exist to get the cells in a specified region of the Grid.
/// Functions exist to get the top left corners of a bunch of cells in a specified region.
///
/// The cell (0,0) represents the top left corner of the grid
/// while the cell (num_x_cells-1,num_y_cells-1) represents the bottom right of the grid.

class Grid {
public:
    Grid() : num_x_cells_(0), num_y_cells_(0), cell_width_(0), cell_height_(0), width_(0), height_(0) {}
    Grid(int num_x_cells, int num_y_cells, int cell_width, int cell_height);

    bool contains(const Node &cell) const;
    QPointF cell_to_point(const Node &cell) const;
    Node point_to_cell(const QPointF &pos) const;

    std::vector<Node> cells_of_column(int i) const;
    std::vector<Node> cells_of_row(int i) const;

    std::vector<Node> cells() const;
    std::vector<Node> cells(const Node &top_left, const Node &bottom_right) const;

    std::vector<Node> cells_in(const QRectF &region) const;
    std::vector<Node> cells_fully_in(const QRectF &region) const;

    std::vector<QPointF> points_of_column(int i) const;
    std::vector<QPointF> points_of_row(int i) const;
    std::vector<QPointF> points() const;

    QRectF cells_to_rect(const Node &cell) const;
    std::vector<QRectF> cells_to_rects(const Node &top_left, const Node &bottom_right) const;

    int num_x_cells() const { return num_x_cells_; }
    int num_y_cells() const { return num_y_cells_; }
    int cell_width() const { return cell_width_; }
    int cell_height() const { return cell_height_; }
    void set_num_x_cells(int v) { num_x_cells_ = v; }
    void set_num_y_cells(int v) { num_y_cells_ = v; }
    void set_cell_width(int v) { cell_width_ = v; }
    void set_cell_height(int v) { cell_height_ = v; }

    int width() const { return width_; }
    int height() const { return height_; }

private:
    int num_x_cells_;
    int num_y_cells_;
    int cell_width_;
    int cell_height_;
    int width_;
    int height_;
};

} // namespace cute
