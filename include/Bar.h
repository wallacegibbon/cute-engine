#pragma once

#include "GUI.h"
#include "Vendor.h"

class QGraphicsPixmapItem;

namespace cute {

class Bar : public GUI {
    Q_OBJECT

public:
    Bar();

    QGraphicsItem *get_graphics_item() override;

    void set_min_value(double minValue);
    double min_value() { return min_value_; }
    void set_max_value(double max_value);
    double max_value() { return max_value_; }
    void set_current_value(double current_value);
    double current_value() { return current_value_; }

    void set_background_color(const QColor &color);
    void set_foreground_color(const QColor &color);
    void set_background_pixmap(const QPixmap &pixmap);
    void set_foreground_pixmap(const QPixmap &pixmap);

    void set_width(double width);
    void set_height(double height);
    void set_vertical_padding(double padding);
    void set_horizontal_padding(double padding);

private:
    std::unique_ptr<QGraphicsPixmapItem> background_pixmap_item_;
    std::unique_ptr<QGraphicsPixmapItem> foreground_pixmap_item_;

    double min_value_ = 0;
    double max_value_ = 100;
    double current_value_ = 40;
    bool values_are_invalid_ = false;

    double width_ = 200;
    double height_ = 10;
    double vertical_padding_ = 2;
    double horizontal_padding_ = 2;

    QColor background_color_ = Qt::gray;
    QColor foreground_color_ = Qt::cyan;
    QPixmap background_pixmap_;
    QPixmap foreground_pixmap_;
    bool is_background_pixmap_ = false;
    bool is_foreground_pixmap_ = false;

    void check_values();
    void draw();
};

} // namespace cute
