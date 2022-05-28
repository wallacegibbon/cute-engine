#pragma once

#include "GUI.h"
#include "Vendor.h"

namespace cute {

/// Represents a label GUI that can be added to a Game.
/// You can set various options such as the font, font size, font color, and width.
/// Oh and you can set the actual text, of course.
/// The Label emits a signal when the mouse hovers over it, when it clicks it, and
/// when it stops hovering over it.
class Label : public GUI, public QGraphicsItem {
    Q_OBJECT

public:
    Label(const std::string &text = "");

    QGraphicsItem *get_graphics_item() override;

    void set_font_size(int fontSize);
    void set_font_color(const QColor &fontColor);
    void set_width(double width);
    void set_text(const std::string &text);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

signals:
    void clicked(Label *label);
    void hovered();
    void unhovered();

private:
    QGraphicsTextItem *text_item_;
    std::string font_;
    int font_size_ = 12;
    QColor fontColor_ = Qt::blue;
    double width_ = 200;
    std::string text_;

    void draw();

    /// override the QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override{};
};

} // namespace cute
