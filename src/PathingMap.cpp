#include "PathingMap.h"
#include "Grid.h"
#include "Utilities.h"

using namespace cute;

/// Fully transparent pixels count as "free" areas, other pixels count as "filled" areas.
PathingMap::PathingMap(const QPixmap &pixmap, int cell_size) : cell_size_(cell_size) {
    QImage image(pixmap.toImage());
    int image_width = image.width();
    int image_height = image.height();

    num_cells_wide_ = image_width / cell_size;
    num_cells_long_ = image_height / cell_size;
    path_grid_ = PathGrid(num_cells_wide_, num_cells_long_);

    Grid grid(num_cells_wide_, num_cells_long_, cell_size, cell_size);
    for (int y = 0; y < image_height; y += 8) {
        for (int x = 0; x < image_width; x += 8) {
            if (qAlpha(image.pixel(x, y)) != 0) {
                fill(grid.point_to_cell(QPointF(x, y)));
            }
        }
    }
}

PathingMap::PathingMap(int num_cells_wide, int num_cells_long, int cell_size)
        : path_grid_(num_cells_wide, num_cells_long), num_cells_wide_(num_cells_wide), num_cells_long_(num_cells_long),
          cell_size_(cell_size) {}

PathingMap::PathingMap() : num_cells_wide_(0), num_cells_long_(0), cell_size_(0) {}

/// Big O is n^2.
std::vector<Node> PathingMap::cells(const Node &top_left, const Node &bottom_right) const {
    return path_grid_.nodes(top_left, bottom_right);
}

std::vector<Node> PathingMap::cells(const QPointF &top_left, const QPointF &bottom_right) const {
    return cells(point_to_cell(top_left), point_to_cell(bottom_right));
}

std::vector<Node> PathingMap::cells(const QRectF &in_region) const {
    return cells(in_region.topLeft(), in_region.bottomRight());
}

/// Big O is n^2.
std::vector<QRectF> PathingMap::cells_as_rects(const Node &top_left, const Node &bottom_right) const {
    std::vector<QRectF> cells_as_rects;
    for (Node node : cells(top_left, bottom_right)) {
        cells_as_rects.push_back(QRectF(cell_to_point(node), QSizeF(cell_size_ - 1, cell_size_ - 1)));
    }
    return cells_as_rects;
}

std::vector<QRectF> PathingMap::cells_as_rects(const QPointF &top_left, const QPointF &bottom_right) const {
    return cells_as_rects(point_to_cell(top_left), point_to_cell(bottom_right));
}

std::vector<QRectF> PathingMap::cells_as_rects(const QRectF &in_region) const {
    return cells_as_rects(in_region.topLeft(), in_region.bottomRight());
}

std::vector<QRectF> PathingMap::cells_as_rects() const {
    return cells_as_rects(Node(0, 0), Node(num_cells_wide_ - 1, num_cells_long_ - 1));
}

std::vector<Node> PathingMap::cells() const {
    return cells(Node(0, 0), Node(num_cells_wide_ - 1, num_cells_long_ - 1));
}

QRectF PathingMap::cell_as_rect(Node cell) const {
    int x = cell.x() * cell_size_;
    int y = cell.y() * cell_size_;
    return QRectF(x, y, cell_size_, cell_size_);
}

bool PathingMap::filled(const Node &cell) const { return path_grid_.filled(cell); }

bool PathingMap::filled(const QPointF &point) const { return filled(point_to_cell(point)); }

/// if `checker(node)` on all elements in the region return true, this function return true
bool PathingMap::check_cells_in_region(const QRectF &region, std::function<bool(Node &)> checker) const {
    for (Node cell : cells(region)) {
        if (!checker(cell)) {
            return false;
        }
    }
    return true;
}

/// Returns true if *all* the cells intersecting with the region are filled.
bool PathingMap::filled(const QRectF &region) const {
    return check_cells_in_region(region, [this](Node &cell) { return filled(cell); });
}

/// Returns true if *all* the cells intersectiong the region are free (not filled).
bool PathingMap::free(const QRectF &region) const {
    return check_cells_in_region(region, [this](Node &cell) { return !filled(cell); });
}

std::vector<QPointF> PathingMap::shortest_path(const Node &from_cell, const Node &to_cell) const {
    std::vector<Node> path = path_grid_.shortest_path(from_cell, to_cell);
    /// scale them up into points
    std::vector<QPointF> points;
    for (Node node : path) {
        points.push_back(cell_to_point(node));
    }
    return points;
}

std::vector<QPointF> PathingMap::shortest_path(const QPointF &p1, const QPointF &p2) const {
    return shortest_path(point_to_cell(p1), point_to_cell(p2));
}

Node PathingMap::point_to_cell(const QPointF &point) const {
    return Node(floor_divide<double>(point.x(), cell_size()), floor_divide<double>(point.y(), cell_size()));
}

QPointF PathingMap::cell_to_point(const Node &cell) const {
    return QPointF(cell.x() * cell_size(), cell.y() * cell_size());
}

/// Returns true if the specified PathingMap can fit in this PathingMap at the specified position.
/// Basically, places the specified PathingMap in this PathingMap at the specified position
/// and sees if it "fits" there without any collision between the filled cells of the two PathingMaps.
bool PathingMap::can_fit(const PathingMap &specified_pathing_map, const QPointF &specified_pos) const {
    if (!contains(specified_pos)) {
        return false;
    }
    /// get only the filled cells
    std::vector<QRectF> filled_cells_as_rects;
    for (QRectF rect : specified_pathing_map.cells_as_rects()) {
        if (specified_pathing_map.filled(rect)) {
            filled_cells_as_rects.push_back(rect);
        }
    }

    /// shift them so they're on the specified_pos on this PathingMap
    std::vector<QRectF> shifted_cells;
    for (QRectF rect : filled_cells_as_rects) {
        rect.moveTopLeft(QPointF(specified_pos.x() + rect.x(), specified_pos.y() + rect.y()));
        shifted_cells.push_back(rect);
    }

    /// if any filled cells intersect with the shifted_cells, return false
    for (QRectF shifted_rect : shifted_cells) {
        for (Node cell : cells(shifted_rect)) {
            if (filled(cell)) {
                return false;
            }
        }
    }

    return true;
}

bool PathingMap::contains(const QPointF &pos) const {
    return pos.x() > 0 && pos.y() > 0 && pos.x() < width() && pos.y() < height();
}

void PathingMap::fill(const Node &cell) { path_grid_.fill(cell); }

void PathingMap::fill(const QPointF &point) { fill(point_to_cell(point)); }

void PathingMap::fill(const Node &top_left, const Node &bottom_right) {
    for (Node cell : cells(top_left, bottom_right)) {
        fill(cell);
    }
}

void PathingMap::fill(const QPointF &top_left, const QPointF &bottom_right) {
    fill(point_to_cell(top_left), point_to_cell(bottom_right));
}

void PathingMap::fill(const QRectF &region) { fill(region.topLeft(), region.bottomRight()); }

void PathingMap::fill() { path_grid_.fill(); }

void PathingMap::unfill(const Node &cell) { path_grid_.unfill(cell); }

void PathingMap::unfill(const QPointF &point) { unfill(point_to_cell(point)); }

void PathingMap::unfill(const Node &top_left, const Node &bottom_right) {
    for (Node cell : cells(top_left, bottom_right)) {
        unfill(cell);
    }
}

void PathingMap::unfill(const QPointF &top_left, const QPointF &bottom_right) {
    unfill(point_to_cell(top_left), point_to_cell(bottom_right));
}

void PathingMap::unfill(const QRectF &region) { unfill(region.topLeft(), region.bottomRight()); }

void PathingMap::unfill() { path_grid_.unfill(); }

/// A value of 0 means unfilled, anything else means fill.
void PathingMap::set_filling(const std::vector<std::vector<int>> &vec) { path_grid_.set_filling(vec); }

/// This works best when the two PathingMaps have the same cell sizes. If the
/// cell sizes are different, the resulting pathing is a little inaccurate.
void PathingMap::set_filling(const PathingMap &another_pathmap, const QPointF &pos) {
    QPointF fixed_pos(cell_to_point(point_to_cell(pos)));

    QRectF unfilled_region(fixed_pos.x(), fixed_pos.y(), another_pathmap.width(), another_pathmap.height());
    unfill(unfilled_region);

    add_filling(another_pathmap, fixed_pos);
}

/// Adds the specified PathingMap to this PathingMap at the specified pos.
///
/// Adding a pathing map "blends" the two PathingMap's filled regions.
/// In other words, the resulting PathingMap can be more filled but never less filled.
void PathingMap::add_filling(const PathingMap &another_pathmap, const QPointF &pos) {
    QPointF fixed_pos(cell_to_point(point_to_cell(pos)));

    std::vector<QRectF> filled_little_rects;
    for (QRectF rect : another_pathmap.cells_as_rects()) {
        if (another_pathmap.filled(rect.topLeft())) {
            rect.moveTopLeft(QPointF(fixed_pos.x() + rect.x(), fixed_pos.y() + rect.y()));
            filled_little_rects.push_back(rect);
        }
    }

    /// the cell size of the another_pathmap may be different from this pathmap, so you need to re-invoke `cells`.
    for (QRectF rect : filled_little_rects) {
        for (Node cell : cells(rect)) {
            fill(cell);
        }
    }
}
