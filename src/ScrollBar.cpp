#include "ScrollBar.h"
#include "QtUtilities.h"

using namespace cute;

ScrollBar::ScrollBar() {
    fg_bar_ = new QGraphicsPixmapItem(this);
    draw();
}

QGraphicsItem *ScrollBar::get_graphics_item() { return this; }

void ScrollBar::set_bg_bar_width(double width) {
    bg_bar_width_ = width;
    draw();
}

void ScrollBar::set_bg_bar_length(double length) {
    bg_bar_length_ = length;
    draw();
}

void ScrollBar::set_fg_bar_width(double width) {
    fg_bar_width_ = width;
    draw();
}

void ScrollBar::set_fg_bar_length(double length) {
    fg_bar_length_ = length;
    draw();
}

/// Sets the length of the "foreground bar" to be a certain fraction of the background bar length.
/// The position of the bar may be moved in order to ensure the newly sized bar fits.
void ScrollBar::set_fg_bar_length_as_fraction_of_bg_bar_length(double fraction) {
    assert(fraction <= 1);

    fg_bar_length_ = fraction * bg_bar_length_;

    /// clamp position if size too big
    if (fg_bar_is_too_high()) {
        fg_bar_position_ = (fg_bar_length_ / 2) / bg_bar_length_;
    } else if (fg_bar_is_too_low()) {
        fg_bar_position_ = (bg_bar_length_ - (fg_bar_length_ / 2)) / bg_bar_length_;
    }

    draw();
}

void ScrollBar::set_bg_bar_color(const QColor &color) {
    is_bg_bar_color_ = true;
    bg_bar_color_ = color;
    draw();
}

void ScrollBar::set_fg_bar_color(const QColor &color) {
    is_fg_bar_color_ = true;
    fg_bar_color_ = color;
    draw();
}

void ScrollBar::set_bg_bar_pixmap(const QPixmap &pixmap) {
    is_bg_bar_color_ = false;
    bg_bar_pixmap_ = pixmap;
    draw();
}

void ScrollBar::set_fg_bar_pixmap(const QPixmap &pixmap) {
    is_fg_bar_color_ = false;
    fg_bar_pixmap_ = pixmap;
    draw();
}

/// "full" means when the length of the foreground and background bar are equivalent.
void ScrollBar::show_even_if_full(bool tf) {
    show_even_if_full_ = tf;
    draw();
}

/// Returns the position of the top of the "foreground bar." The position
/// is 0 to 1, 0 being the top of the background bar, 1 being the very bottom.
/// Note that the position of the *top* of the foreground bar can not be exactly
/// 1 unless the length of the foreground bar is 0 (because the *top* of the
/// foreground bar will never touch the very bottom of the background bar, unless
/// the foreground bar's length is 0).
double ScrollBar::fg_bar_top_pos() {
    double fg_abs_center = fg_bar_position_ * bg_bar_length_;
    double fg_abs_top = fg_abs_center - fg_bar_length_ / 2;
    double fg_frac_top = fg_abs_top / bg_bar_length_;

    return fg_frac_top;
}

double ScrollBar::fg_bar_bottom_pos() {
    double fg_abs_center = fg_bar_position_ * bg_bar_length_;
    double fg_abs_bottom = fg_abs_center + fg_bar_length_ / 2;
    double fg_frac_bottom = fg_abs_bottom / bg_bar_length_;

    return fg_frac_bottom;
}

void ScrollBar::mousePressEvent(QGraphicsSceneMouseEvent *event) { fg_bar_is_being_dragged_ = true; }

void ScrollBar::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { fg_bar_is_being_dragged_ = false; }

void ScrollBar::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    /// approach:
    /// -get pos of mouse from 0 - 1
    /// -set that as bar position
    /// -clam according to size

    if (!fg_bar_is_being_dragged_) {
        return;
    }

    fg_bar_position_ = event->pos().y() / bg_bar_length_;

    /// clamp fg_bar_position_
    if (fg_bar_is_too_high()) {
        fg_bar_position_ = (fg_bar_length_ / 2) / bg_bar_length_;
    } else if (fg_bar_is_too_low()) {
        fg_bar_position_ = (bg_bar_length_ - (fg_bar_length_ / 2)) / bg_bar_length_;
    }
    draw();
    emit position_changed(fg_bar_position_);
}

void ScrollBar::draw() {
    /// if the bar is full, make it 100% transparent (i.e. "don't show it")
    const double EPSILON = 3;
    if (qAbs(fg_bar_length_ - bg_bar_length_) < EPSILON && !show_even_if_full_) {
        QColor transparent(Qt::transparent);
        setPixmap(QtUtils::pixmap_from_color(QSize(bg_bar_width_, bg_bar_length_), transparent));
        fg_bar_->setPixmap(QtUtils::pixmap_from_color(QSize(fg_bar_width_, fg_bar_length_), transparent));
        return;
    }

    /// draw the background bar
    if (is_bg_bar_color_) {
        setPixmap(QtUtils::pixmap_from_color(QSize(bg_bar_width_, bg_bar_length_), bg_bar_color_));
    } else {
        setPixmap(bg_bar_pixmap_);
    }
    /// draw the foreground bar
    /// size/color
    if (is_fg_bar_color_) {
        fg_bar_->setPixmap(QtUtils::pixmap_from_color(QSize(fg_bar_width_, fg_bar_length_), fg_bar_color_));
    } else {
        fg_bar_->setPixmap(fg_bar_pixmap_);
    }
    /// position foreground bar
    double fg_bar_x = (bg_bar_width_ - fg_bar_width_) / 2;
    double fg_bar_y = fg_bar_position_ * bg_bar_length_ - fg_bar_length_ / 2;
    fg_bar_->setPos(fg_bar_x, fg_bar_y);
}

bool ScrollBar::fg_bar_is_too_high() { return fg_bar_position_ * bg_bar_length_ - fg_bar_length_ / 2 < 0; }

bool ScrollBar::fg_bar_is_too_low() { return fg_bar_position_ * bg_bar_length_ + fg_bar_length_ / 2 > bg_bar_length_; }
