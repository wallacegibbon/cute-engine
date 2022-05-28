#pragma once

#include "Grid.h"
#include "Vendor.h"

namespace cute {

class Node;

class SpriteSheet {
public:
    SpriteSheet(std::string file_path, int rows, int columns, double tile_width, double tile_height);

    QPixmap tile_at(const Node &cell) const;
    std::vector<QPixmap> tiles_at(const Node &top_left, const Node &bottom_right) const;

    int rows() const;
    int columns() const;
    double tile_width() const;
    double tile_height() const;

private:
    Grid grid_;
    QPixmap pixmap_;
};

} // namespace cute
