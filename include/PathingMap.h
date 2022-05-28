#pragma once

#include "PathGrid.h"
#include "Vendor.h"

namespace cute {

/// Represents a rectangular region of space divided into square cells where
/// each cell can either be filled or unfilled.

class PathingMap {
public:
    PathingMap(int num_cells_wide, int num_cells_long, int cell_size);
    PathingMap(const QPixmap &pixmap, int cell_size);
    PathingMap();

    /// make compiler generate default copy ctor
    /// Q_DECLARE_METATYPE needs public ctor, dtor, copy-ctor
    PathingMap(const PathingMap &copy) = default;

    std::vector<Node> cells(const Node &top_left, const Node &bottom_right) const;
    std::vector<Node> cells(const QPointF &top_left, const QPointF &bottom_right) const;
    std::vector<Node> cells(const QRectF &in_region) const;
    std::vector<Node> cells() const;

    QRectF cell_as_rect(Node cell) const;
    std::vector<QRectF> cells_as_rects(const Node &top_left, const Node &bottom_right) const;
    std::vector<QRectF> cells_as_rects(const QPointF &top_left, const QPointF &bottom_right) const;
    std::vector<QRectF> cells_as_rects(const QRectF &in_region) const;
    std::vector<QRectF> cells_as_rects() const;

    bool filled(const Node &cell) const;
    bool filled(const QPointF &point) const;
    bool filled(const QRectF &region) const;

    bool check_cells_in_region(const QRectF &region, std::function<bool(Node &)>) const;

    bool free(const QRectF &region) const;

    std::vector<QPointF> shortest_path(const Node &fromCell, const Node &toCell) const;
    std::vector<QPointF> shortest_path(const QPointF &fromPt, const QPointF &toPt) const;

    int width() const { return cell_size_ * num_cells_wide_; }
    int height() const { return cell_size_ * num_cells_long_; }

    int cell_size() const { return cell_size_; }

    int num_cells_long() const { return num_cells_long_; }
    int num_cells_wide() const { return num_cells_wide_; }

    Node point_to_cell(const QPointF &point) const;
    QPointF cell_to_point(const Node &cell) const;

    bool can_fit(const PathingMap &specifiedPathingMap, const QPointF &specifiedPos) const;
    bool contains(const QPointF &pos) const;

    void fill(const Node &cell);
    void fill(const QPointF &point);
    void fill(const Node &top_left, const Node &bottom_right);
    void fill(const QPointF &top_left, const QPointF &bottom_right);
    void fill(const QRectF &region);
    void fill();

    void unfill(const Node &cell);
    void unfill(const QPointF &point);
    void unfill(const Node &top_left, const Node &bottom_right);
    void unfill(const QPointF &top_left, const QPointF &bottom_right);
    void unfill(const QRectF &region);
    void unfill();

    void set_filling(const std::vector<std::vector<int>> &vec);
    void set_filling(const PathingMap &another_pathmap, const QPointF &pos);
    void add_filling(const PathingMap &another_pathmap, const QPointF &pos);

private:
    PathGrid path_grid_;
    int num_cells_wide_;
    int num_cells_long_;
    int cell_size_;
};

} // namespace cute

Q_DECLARE_METATYPE(cute::PathingMap);
