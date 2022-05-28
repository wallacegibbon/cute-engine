#include "Grid.h"
#include "Node.h"
#include "Utilities.h"

using namespace cute;

Grid::Grid(int num_x_cells, int num_y_cells, int cell_width, int cell_height)
        : num_x_cells_(num_x_cells), num_y_cells_(num_y_cells), cell_width_(cell_width), cell_height_(cell_height) {

    width_ = num_x_cells * cell_width;
    height_ = num_y_cells * cell_height;
}

bool Grid::contains(const Node &cell) const {
    return cell.x() >= 0 && cell.y() >= 0 && cell.x() < num_x_cells() && cell.y() < num_y_cells();
}

/// returns the position of the top left corner of the specified cell.
QPointF Grid::cell_to_point(const Node &cell) const {
    /// make sure the Node is in the Grid
    assert(cell.x() < num_x_cells() && cell.y() < num_y_cells());
    return QPointF(cell.x() * cell_width(), cell.y() * cell_height());
}

Node Grid::point_to_cell(const QPointF &pos) const {
    /// make sure the point is in the Grid
    assert(int(pos.x()) < width() && int(pos.y()) < height());
    return Node(floor_divide<double>(pos.x(), cell_width()), floor_divide<double>(pos.y(), cell_height()));
}

std::vector<Node> Grid::cells_of_column(int column) const {
    assert(column < num_x_cells());
    std::vector<Node> nodes_of_column;
    for (int y = 0, n = num_y_cells(); y < n; y++) {
        nodes_of_column.push_back(Node(column, y));
    }
    return nodes_of_column;
}

std::vector<Node> Grid::cells_of_row(int row) const {
    assert(row < num_y_cells());
    std::vector<Node> nodes_of_row;
    for (int x = 0, n = num_x_cells(); x < n; x++) {
        nodes_of_row.push_back(Node(x, row));
    }
    return nodes_of_row;
}

std::vector<Node> Grid::cells() const {
    std::vector<Node> all_nodes;
    for (int y = 0, n = num_y_cells(); y < n; y++) {
        for (Node node : cells_of_row(y)) {
            all_nodes.push_back(node);
        }
    }
    return all_nodes;
}

std::vector<Node> Grid::cells(const Node &top_left, const Node &bottom_right) const {
    std::vector<Node> nodes_in_region;
    for (int y = top_left.y(), n = bottom_right.y(); y <= n; y++) {
        for (int x = top_left.x(), p = bottom_right.x(); x <= p; x++) {
            if (contains(Node(x, y))) {
                nodes_in_region.push_back(Node(x, y));
            }
        }
    }
    return nodes_in_region;
}

std::vector<Node> Grid::cells_in(const QRectF &region) const {
    Node top_left = point_to_cell(region.topLeft());
    Node bottom_right = point_to_cell(region.bottomRight());
    return cells(top_left, bottom_right);
}

std::vector<Node> Grid::cells_fully_in(const QRectF &region) const {
    Node top_left = point_to_cell(region.topLeft());
    Node bottom_right = point_to_cell(region.bottomRight());
    top_left.set_x(top_left.x() + 1);
    top_left.set_y(top_left.y() + 1);
    bottom_right.set_x(bottom_right.x() - 1);
    bottom_right.set_y(bottom_right.y() - 1);
    return cells(top_left, bottom_right);
}

/// Returns the (top left) positions of all the cells of the specified column.
std::vector<QPointF> Grid::points_of_column(int column) const {
    std::vector<QPointF> points_of_column;
    for (Node node : cells_of_column(column)) {
        points_of_column.push_back(cell_to_point(node));
    }
    return points_of_column;
}

std::vector<QPointF> Grid::points_of_row(int row) const {
    std::vector<QPointF> points_of_row;
    for (Node node : cells_of_row(row)) {
        points_of_row.push_back(cell_to_point(node));
    }
    return points_of_row;
}

std::vector<QPointF> Grid::points() const {
    std::vector<QPointF> all_points;
    for (Node node : cells()) {
        all_points.push_back(cell_to_point(node));
    }
    return all_points;
}

QRectF Grid::cells_to_rect(const Node &cell) const {
    QRectF rect;
    rect.setX(cell.x() * cell_width());
    rect.setY(cell.y() * cell_height());
    rect.setWidth(cell_width());
    rect.setHeight(cell_height());

    return rect;
}

std::vector<QRectF> Grid::cells_to_rects(const Node &top_left, const Node &bottom_right) const {
    std::vector<QRectF> cells_as_rects;
    for (Node node : cells(top_left, bottom_right)) {
        cells_as_rects.push_back(cells_to_rect(node));
    }
    return cells_as_rects;
}
