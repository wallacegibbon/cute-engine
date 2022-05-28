#include "Bar.h"

using namespace cute;

Bar::Bar() : background_pixmap_(), foreground_pixmap_() {
    background_pixmap_item_.reset(new QGraphicsPixmapItem());
    foreground_pixmap_item_.reset(new QGraphicsPixmapItem(background_pixmap_item_.get()));
    draw();
}

QGraphicsItem *Bar::get_graphics_item() { return background_pixmap_item_.get(); }

void Bar::set_min_value(double minValue) {
    min_value_ = minValue;
    check_values();
    draw();
}

void Bar::set_max_value(double max_value) {
    max_value_ = max_value;
    check_values();
    draw();
}

void Bar::set_current_value(double current_value) {
    current_value_ = current_value;
    check_values();
    draw();
}

void Bar::set_background_color(const QColor &color) {
    background_color_ = color;
    is_background_pixmap_ = false;
    draw();
}

void Bar::set_foreground_color(const QColor &color) {
    foreground_color_ = color;
    is_foreground_pixmap_ = false;
    draw();
}

void Bar::set_background_pixmap(const QPixmap &pixmap) {
    background_pixmap_ = pixmap;
    is_background_pixmap_ = true;
    draw();
}

void Bar::set_foreground_pixmap(const QPixmap &pixmap) {
    foreground_pixmap_ = pixmap;
    is_foreground_pixmap_ = true;
    draw();
}

void Bar::set_width(double width) {
    width_ = width;
    draw();
}

void Bar::set_height(double height) {
    height_ = height;
    draw();
}

void Bar::set_vertical_padding(double padding) {
    vertical_padding_ = padding;
    draw();
}

void Bar::set_horizontal_padding(double padding) {
    horizontal_padding_ = padding;
    draw();
}

void Bar::check_values() {
    /// clamp up/down if needed
    if (current_value_ > max_value_) {
        current_value_ = max_value_;
    } else if (current_value_ < min_value_) {
        current_value_ = min_value_;
    }

    /// see if values are invalid (won't be drawn)
    values_are_invalid_ = min_value_ > max_value_;
}

void Bar::draw() {
    /// if the values are invalid, draw emptyness
    if (values_are_invalid_) {
        background_pixmap_item_->setPixmap(QPixmap());
        foreground_pixmap_item_->setPixmap(QPixmap());
        return;
    }

    /// draw background
    double fg_width = (width_ - 2 * horizontal_padding_) * (current_value_ - min_value_) / (max_value_ - min_value_);
    double fg_height = height_ - 2 * vertical_padding_;
    QPointF fg_pos(horizontal_padding_, vertical_padding_);
    if (is_background_pixmap_) {
        background_pixmap_ = background_pixmap_.scaled(width_, height_);
        foreground_pixmap_ = foreground_pixmap_.scaled(fg_width, fg_height);

        background_pixmap_item_->setPixmap(background_pixmap_);
        foreground_pixmap_item_->setPixmap(foreground_pixmap_);
    } else {
        QImage bg_img(QSize(width_, height_), QImage::Format_RGB32);
        bg_img.fill(background_color_);
        background_pixmap_item_->setPixmap(QPixmap::fromImage(bg_img));

        QImage fg_img(QSize(fg_width, fg_height), QImage::Format_RGB32);
        fg_img.fill(foreground_color_);
        foreground_pixmap_item_->setPixmap(QPixmap::fromImage(fg_img));
    }

    foreground_pixmap_item_->setPos(fg_pos);
}
