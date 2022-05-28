#include "Panel.h"
#include "QtUtilities.h"

using namespace cute;

Panel::Panel() : border_(new QGraphicsRectItem()) {
    setAcceptHoverEvents(true);
    border_->setParentItem(this);
    draw();
}

QGraphicsItem *Panel::get_graphics_item() { return this; }

void Panel::set_background_color(const QColor &color) {
    background_color_ = color;
    is_background_pixmap_ = false;
    draw();
}

void Panel::set_background_pixmap(const QPixmap &pixmap) {
    background_pixmap_ = pixmap;
    is_background_pixmap_ = true;
    draw();
}

void Panel::set_width(double width) {
    width_ = width;
    draw();
}

void Panel::set_height(double height) {
    height_ = height;
    draw();
}

void Panel::show_border(bool tf) {
    show_border_ = tf;
    draw();
}

void Panel::set_border_thickness(double thickness) {
    border_thickness_ = thickness;
    draw();
}

void Panel::set_border_color(const QColor &color) {
    border_color_ = color;
    draw();
}

void Panel::show_background(bool tf) {
    show_background_ = tf;
    draw();
}

void Panel::mousePressEvent(QGraphicsSceneMouseEvent *event) { emit clicked(this, event->pos(), event->button()); }

void Panel::hoverEnterEvent(QGraphicsSceneHoverEvent *event) { emit mouse_hover_started(this, event->pos()); }

void Panel::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { emit mouse_hover_ended(this, event->pos()); }

void Panel::hoverMoveEvent(QGraphicsSceneHoverEvent *event) { emit mouse_hover_moved(this, event->pos()); }

void Panel::draw() {
    border_->setRect(0, 0, width_, height_);
    QPen border_pen;
    border_pen.setWidth(border_thickness_);
    border_pen.setColor(border_color_);
    border_->setPen(border_pen);
    if (show_border_) {
        border_->setVisible(true);
    } else {
        border_->setVisible(false);
    }
    setPixmap(QtUtils::pixmap_from_color(QSize(width_, height_), Qt::transparent));
    if (!show_background_) {
        return;
    }
    if (is_background_pixmap_) {
        background_pixmap_ = background_pixmap_.scaled(width_, height_);
        setPixmap(background_pixmap_);
    } else {
        setPixmap(QtUtils::pixmap_from_color(QSize(width_, height_), background_color_));
    }
}
