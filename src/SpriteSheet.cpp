#include "SpriteSheet.h"
#include "Node.h"

using namespace cute;

SpriteSheet::SpriteSheet(std::string file_path, int rows, int columns, double tile_width, double tile_height)
        : grid_(rows, columns, tile_width, tile_height) {
    pixmap_.load(QString::fromStdString(file_path));
}

QPixmap SpriteSheet::tile_at(const Node &cell) const {
    QRect tile_region;
    tile_region.setX(cell.x() * tile_width());
    tile_region.setY(cell.y() * tile_height());
    tile_region.setWidth(tile_width());
    tile_region.setHeight(tile_height());

    return pixmap_.copy(tile_region);
}

std::vector<QPixmap> SpriteSheet::tiles_at(const Node &top_left, const Node &bottom_right) const {
    std::vector<Node> cells = grid_.cells(top_left, bottom_right);
    std::vector<QPixmap> tiles;
    for (Node cell : cells) {
        tiles.push_back(tile_at(cell));
    }
    return tiles;
}

int SpriteSheet::rows() const { return grid_.num_x_cells(); }

int SpriteSheet::columns() const { return grid_.num_y_cells(); }

double SpriteSheet::tile_width() const { return grid_.cell_width(); }

double SpriteSheet::tile_height() const { return grid_.cell_height(); }
