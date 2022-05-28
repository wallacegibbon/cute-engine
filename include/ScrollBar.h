#pragma once

#include "GUI.h"
#include "Vendor.h"

class QGraphicsSceneMouseEvent;
class QGraphicsItem;

namespace cute {

/// Represents a GUI that has a little dragable bar ( called a "foreground bar")
/// that you can drag from one end to the other on a "background bar".
/// The ScrollBar will emit a position_changed event whenever the foreground bar
/// is dragged. The position can be from 0 to 1, 0 being in the beggining and 1
/// being all the way at the end.
///
/// Note: If the bar is full (i.e. length of background and foreground bars are the same),
/// the ScrollBar will not show up by default. To have it show up anyways,
/// call showEvenIfFull(true);
/// @author Abdullah Aghazadah
/// @date 10-11-16
class ScrollBar : public GUI, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    ScrollBar();

    QGraphicsItem *get_graphics_item() override;

    void set_bg_bar_width(double width);
    void set_bg_bar_length(double length);
    void set_fg_bar_width(double width);
    void set_fg_bar_length(double length);
    void set_fg_bar_length_as_fraction_of_bg_bar_length(double fraction);
    void set_bg_bar_color(const QColor &color);
    void set_fg_bar_color(const QColor &color);
    void set_bg_bar_pixmap(const QPixmap &pixmap);
    void set_fg_bar_pixmap(const QPixmap &pixmap);
    void show_even_if_full(bool tf);

    double fg_bar_center_pos() { return fg_bar_position_; }
    double fg_bar_top_pos();
    double fg_bar_bottom_pos();

    double fg_bar_width() { return fg_bar_width_; }
    double fg_bar_length() { return fg_bar_length_; }
    double bg_bar_width() { return bg_bar_width_; }
    double bg_bar_length() { return bg_bar_length_; }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    /// Emitted when the position of the dragable bar changes.
    /// The position is from 0-1, 0 being at the very beginning and 1 being at the very end.
    /// The position is the position of the *center* of the dragable bar.
    void position_changed(double newPosition);

private:
    double bg_bar_width_ = 20;
    double bg_bar_length_ = 250;
    double fg_bar_width_ = 15;
    double fg_bar_length_ = 35;
    bool is_bg_bar_color_ = true;
    bool is_fg_bar_color_ = true;
    QColor bg_bar_color_ = Qt::darkGray;
    QColor fg_bar_color_ = Qt::lightGray;
    QPixmap bg_bar_pixmap_;
    QPixmap fg_bar_pixmap_;
    bool show_even_if_full_ = false;

    double fg_bar_position_ = 0;

    bool fg_bar_is_being_dragged_;

    QGraphicsPixmapItem *fg_bar_;

private:
    void draw();
    bool fg_bar_is_too_high();
    bool fg_bar_is_too_low();
};

} // namespace cute
