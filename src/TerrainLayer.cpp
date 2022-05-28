#include "TerrainLayer.h"
#include "Node.h"
#include "stl_helper.h"

using namespace cute;

void TerrainLayer::set_alpha_channel(QPixmap &pixmap, int const alpha) {
    QImage image(pixmap.toImage().convertToFormat(QImage::Format_ARGB32));

    for (int x(0); x != image.width(); x++) {
        for (int y(0); y != image.height(); y++) {
            QColor color(image.pixel(x, y));
            color.setAlpha(alpha);
            image.setPixel(x, y, color.rgba());
        }
    }
    pixmap = QPixmap::fromImage(image);
}

/// Constructs a TerrainLayer with the specified number of tiles in the x and y direction.
/// The pixmap is a default grassy picture and everything is filled.
/// Use unfill() to unfill some cells and use setPixmap() to change the pixmap.
TerrainLayer::TerrainLayer(int num_x_tiles, int num_y_tiles)
        : TerrainLayer(num_x_tiles, num_y_tiles,
                       QPixmap(":/cute-engine-builtin/resources/graphics/terrain/grass.png")) {}

/// Constructs a TerrainLayer with the specified number of tiles in the x and y direction.
/// Each filled cell of the TerrainLayer will contain the specified pixmap.
/// By default, every single cell is marked as filled (use unfill() to unfill some of the cells).
TerrainLayer::TerrainLayer(int num_x_tiles, int num_y_tiles, QPixmap pixmap)
        : num_x_tiles_(num_x_tiles), num_y_tiles_(num_y_tiles), pixmap_(pixmap) {

    parent_item_ = new QGraphicsPixmapItem();
    tile_width_ = pixmap.width();
    tile_height_ = pixmap.height();

    grid_ = Grid(num_x_tiles_, num_y_tiles_, tile_width_, tile_height_);

    /// initialize all pixmaps to nullptr
    for (int i = 0, n = num_x_tiles_ * num_y_tiles_; i < n; i++) {
        tiles_.push_back(nullptr);
    }

    linear_fade_percent_ = 0.2; /// 20%
    fill();
}

void TerrainLayer::set_pos(const QPointF &pos) { parent_item_->setPos(pos); }

void TerrainLayer::set_pixmap(QPixmap pixmap) {
    pixmap_ = pixmap;
    draw();
}

void TerrainLayer::set_tile_width(double width) {
    tile_width_ = width;
    grid_ = Grid(num_x_tiles_, num_y_tiles_, tile_width_, tile_height_);
    draw();
}

void TerrainLayer::set_tile_height(double height) {
    grid_ = Grid(num_x_tiles_, num_y_tiles_, tile_width_, tile_height_);
    tile_height_ = height;
    draw();
}

void TerrainLayer::fill(const Node &cell) {
    QGraphicsPixmapItem *pixmap = pixmap_at(cell.x(), cell.y());

    /// do nothing if already filled
    if (pixmap != nullptr) {
        return;
    }

    int pos_in_array = cell.y() * num_x_tiles_ + cell.x();
    tiles_[pos_in_array] = new QGraphicsPixmapItem(parent_item_);
    draw();
}

void TerrainLayer::unfill(const Node &cell) {
    int pos_in_array = cell.y() * num_x_tiles_ + cell.x();
    QGraphicsPixmapItem *tile = tiles_[pos_in_array];

    /// do nothing if already unfilled
    if (tile == nullptr) {
        return;
    }

    delete tile;
    tiles_[pos_in_array] = nullptr;
    draw();
}

bool TerrainLayer::filled(const Node &cell) {
    int pos_in_array = cell.y() * num_x_tiles_ + cell.x();
    QGraphicsPixmapItem *tile = tiles_[pos_in_array];

    return tile != nullptr;
}

void TerrainLayer::fill() {
    for (Node cell : grid_.cells()) {
        fill(cell);
    }
}

/// Use to enable/disable the fading of edges/corners of the cells in the TerrainLayer.
void TerrainLayer::set_enable_fade(bool flag) {
    enable_fade_ = flag;
    for (Node cell : grid_.cells()) {
        set_correct_fade(cell.x(), cell.y());
    }
}

/// Returns the pixmap at the specified location. If there is none, returns nullptr.
QGraphicsPixmapItem *TerrainLayer::pixmap_at(int x, int y) {
    if (x < 0 || x >= num_x_tiles_ || y < 0 || y >= num_y_tiles_) {
        return nullptr;
    }
    int pos_in_array = y * num_x_tiles_ + x;
    return tiles_[pos_in_array];
}

/// Returns the position (node) of the specified QGraphicsPixmapItem.
Node TerrainLayer::position_of(QGraphicsPixmapItem *pixmap_item) {
    int index_of_pixmap = stl_helper::find_index(tiles_, pixmap_item);
    assert(index_of_pixmap != -1);
    return Node(index_of_pixmap % num_y_tiles_, index_of_pixmap / num_y_tiles_);
}

/// Executed whenver the specified cell needs to recalculate itsfade.
void TerrainLayer::set_correct_fade(int x, int y) {
    if (!enable_fade_) {
        pixmap_at(x, y)->setGraphicsEffect(nullptr);
        return;
    }

    /// find out if its top, left, middle, etc...
    bool is_top_left = false;
    bool is_top_right = false;
    bool is_bottom_left = false;
    bool is_bottom_right = false;

    bool is_top = false;
    bool is_bottom = false;
    bool is_left = false;
    bool is_right = false;

    bool is_middle = false;

    QGraphicsPixmapItem *top_left = pixmap_at(x - 1, y - 1);
    QGraphicsPixmapItem *top_right = pixmap_at(x + 1, y - 1);
    QGraphicsPixmapItem *bottom_left = pixmap_at(x - 1, y + 1);
    QGraphicsPixmapItem *bottom_right = pixmap_at(x + 1, y + 1);

    QGraphicsPixmapItem *top = pixmap_at(x, y - 1);
    QGraphicsPixmapItem *bottom = pixmap_at(x, y + 1);
    QGraphicsPixmapItem *left = pixmap_at(x - 1, y);
    QGraphicsPixmapItem *right = pixmap_at(x + 1, y);

    /// see where this cell is
    is_top_left = !top && !left && right && bottom;
    is_top_right = !top && !right && bottom && left;
    is_bottom_left = !left && !bottom && top && right;
    is_bottom_right = !right && !bottom && top && left;
    is_top = !top && right && left && bottom;
    is_bottom = !bottom && right && left && top;
    is_left = !left && right && top && bottom;
    is_right = !right && left && top && bottom;
    is_middle = top && bottom && left && right;

    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);
    if (!pixmap_item) {
        return;
    }

    if (is_middle) {
        pixmap_item->setGraphicsEffect(nullptr);
    } else if (is_top_left) {
        fade_top_left_only(x, y);
    } else if (is_top_right) {
        fade_top_right_only(x, y);
    } else if (is_bottom_left) {
        fade_bottom_left_only(x, y);
    } else if (is_bottom_right) {
        fade_bottom_right_only(x, y);
    } else if (is_left) {
        fade_left_only(x, y);
    } else if (is_right) {
        fade_right_only(x, y);
    } else if (is_top) {
        fade_top_only(x, y);
    } else if (is_bottom) {
        fade_bottom_only(x, y);
    }
}

void TerrainLayer::fade_left_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();
    QLinearGradient lg;
    lg.setStart(linear_fade_percent_ * pixmap_item->boundingRect().width(), 0);
    lg.setFinalStop(0, 0);
    lg.setColorAt(0, Qt::blue);
    lg.setColorAt(1, Qt::transparent);
    opacity->setOpacityMask(lg);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::fade_right_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();
    QLinearGradient lg;
    lg.setStart(pixmap_item->boundingRect().width() - linear_fade_percent_ * pixmap_item->boundingRect().width(), 0);
    lg.setFinalStop(pixmap_item->boundingRect().width(), 0);
    lg.setColorAt(0, Qt::black);
    lg.setColorAt(1, Qt::transparent);
    opacity->setOpacityMask(lg);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::fade_top_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();
    QLinearGradient lg;
    lg.setStart(0, linear_fade_percent_ * pixmap_item->boundingRect().width());
    lg.setFinalStop(0, 0);
    lg.setColorAt(0, Qt::black);
    lg.setColorAt(1, Qt::transparent);
    opacity->setOpacityMask(lg);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::fade_bottom_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();
    QLinearGradient lg;
    lg.setStart(0, pixmap_item->boundingRect().height() - linear_fade_percent_ * pixmap_item->boundingRect().width());
    lg.setFinalStop(0, pixmap_item->boundingRect().height());
    lg.setColorAt(0, Qt::black);
    lg.setColorAt(1, Qt::transparent);
    opacity->setOpacityMask(lg);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::fade_top_left_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();

    QRadialGradient radial_gradient;
    radial_gradient.setCenter(pixmap_item->boundingRect().width(), pixmap_item->boundingRect().height());
    radial_gradient.setFocalPoint(radial_gradient.center());
    radial_gradient.setRadius(pixmap_item->boundingRect().width());
    radial_gradient.setColorAt(0, Qt::black);
    radial_gradient.setColorAt(0.8, Qt::black);
    radial_gradient.setColorAt(1, Qt::transparent);

    opacity->setOpacityMask(radial_gradient);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::fade_top_right_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();

    QRadialGradient radial_gradient;
    radial_gradient.setCenter(0, pixmap_item->boundingRect().height());
    radial_gradient.setFocalPoint(radial_gradient.center());
    radial_gradient.setRadius(pixmap_item->boundingRect().width());
    radial_gradient.setColorAt(0, Qt::black);
    radial_gradient.setColorAt(0.8, Qt::black);
    radial_gradient.setColorAt(1, Qt::transparent);

    opacity->setOpacityMask(radial_gradient);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::fade_bottom_left_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();

    QRadialGradient radial_gradient;
    radial_gradient.setCenter(pixmap_item->boundingRect().width(), 0);
    radial_gradient.setFocalPoint(radial_gradient.center());
    radial_gradient.setRadius(pixmap_item->boundingRect().width());
    radial_gradient.setColorAt(0, Qt::black);
    radial_gradient.setColorAt(0.8, Qt::black);
    radial_gradient.setColorAt(1, Qt::transparent);

    opacity->setOpacityMask(radial_gradient);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::fade_bottom_right_only(int x, int y) {
    QGraphicsPixmapItem *pixmap_item = pixmap_at(x, y);

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect();

    QRadialGradient radial_gradient;
    radial_gradient.setCenter(0, 0);
    radial_gradient.setFocalPoint(radial_gradient.center());
    radial_gradient.setRadius(pixmap_item->boundingRect().width());
    radial_gradient.setColorAt(0, Qt::black);
    radial_gradient.setColorAt(0.8, Qt::black);
    radial_gradient.setColorAt(1, Qt::transparent);

    opacity->setOpacityMask(radial_gradient);
    opacity->setOpacity(1);

    pixmap_item->setGraphicsEffect(opacity);
}

void TerrainLayer::draw() {
    /// scale the pixmap
    pixmap_ = pixmap_.scaled(tile_width_, tile_height_);

    // set_alpha_channel(pixmap_, 254); /// TODO: why was this here? why is it disabled now?

    for (QGraphicsPixmapItem *pixmap_item : tiles_) {
        if (pixmap_item) {
            Node pos = position_of(pixmap_item);
            pixmap_item->setPixmap(pixmap_);
            pixmap_item->setPos(grid_.cell_to_point(pos));
            set_correct_fade(pos.x(), pos.y());
        }
    }
}
