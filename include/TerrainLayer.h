#pragma once

#include "Grid.h"
#include "Node.h"
#include "Vendor.h"

namespace cute {

class Map;

/// Represents a grid with a bunch of cells; each of its cells can contain a QPixmap or it can be empty.
///
/// This grid can be placed in a Map by using the Map's `add_terrain_layer()` method.
/// QPixmaps in a grid will be nicely blended.
///
/// Example
/// -------
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// TerrainLayer* tl = new TerrainLayer(10,10,my_pixmap);
/// tl->set_pos(QPointF(100,100));
/// Map* map;
/// map->add_terrain_layer(tl);
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
/// @note You cannot place 2 *different* QPixmaps in the same TerrainLayer, you choose one QPixmap,
/// and then you mark a cell as either filled or not.
/// If a cell is marked as filled, the QPixmap will appear there, otherwise it won't.
/// You can add multiple TerrainLayers in a Map to compensate for this fact.
///
/// Functions exist for setting the pixmap to be used, for filling/unfilling cells, for setting the size of cells,
/// and for setting the position of the top left corner of the TerrainLayer in the Map.
/// If you want to disable the nice blending/fading, call set_enable_fade(false).

class TerrainLayer {
    friend class Map;

public:
    static void set_alpha_channel(QPixmap &pixmap, int const alpha);

    TerrainLayer(int num_x_tiles, int num_y_tiles);
    TerrainLayer(int num_x_tiles, int num_y_tiles, QPixmap pixmap);

    void set_pos(const QPointF &pos);
    QPointF pos() { return pos_; }
    void set_pixmap(QPixmap pixmap);
    void set_tile_width(double width);
    void set_tile_height(double height);

    void fill(const Node &cell);
    void unfill(const Node &cell);
    bool filled(const Node &cell);
    void fill();

    void set_enable_fade(bool flag);

private:
    QGraphicsPixmapItem *pixmap_at(int x, int y);
    Node position_of(QGraphicsPixmapItem *pixmap_item);
    void set_correct_fade(int x, int y);
    void fade_left_only(int x, int y);
    void fade_right_only(int x, int y);
    void fade_top_only(int x, int y);
    void fade_bottom_only(int x, int y);
    void fade_top_left_only(int x, int y);
    void fade_top_right_only(int x, int y);
    void fade_bottom_left_only(int x, int y);
    void fade_bottom_right_only(int x, int y);
    void draw();

private:
    int num_x_tiles_;
    int num_y_tiles_;
    int tile_width_;
    int tile_height_;
    QPixmap pixmap_;
    QPointF pos_;
    std::vector<QGraphicsPixmapItem *> tiles_;
    bool enable_fade_ = true;

    /// constants (TODO: make variable)
    double linear_fade_percent_;

    Grid grid_;
    QGraphicsPixmapItem *parent_item_;
};

} // namespace cute
