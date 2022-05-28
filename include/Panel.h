#pragma once

#include "GUI.h"
#include "Vendor.h"

class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;

namespace cute {

/// The panel will emit events when the mouse hovers/unhovers over it and when the mouse clicks it.
class Panel : public GUI, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    Panel();

    void set_width(double width);
    void set_height(double height);
    void show_background(bool tf);
    void set_background_color(const QColor &color);
    void set_background_pixmap(const QPixmap &pixmap);
    void show_border(bool tf);
    void set_border_thickness(double thickness);
    void set_border_color(const QColor &color);

    int height() { return height_; }
    int width() { return width_; }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    QGraphicsItem *get_graphics_item() override;

signals:
    void clicked(Panel *panel, QPointF pos, int button);
    void mouse_hover_started(Panel *panel, QPointF pos);
    void mouse_hover_ended(Panel *panel, QPointF pos);
    void mouse_hover_moved(Panel *panel, QPointF pos);

private:
    std::unique_ptr<QGraphicsRectItem> border_;
    QPixmap background_pixmap_;
    QColor background_color_ = Qt::gray;
    bool is_background_pixmap_ = false;
    bool show_background_ = true;
    bool show_border_ = true;
    double border_thickness_ = 3;
    QColor border_color_ = Qt::black;
    double width_ = 300;
    double height_ = 300;

    void draw();
};

} // namespace cute
