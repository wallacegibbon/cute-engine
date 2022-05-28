#include "Label.h"

using namespace cute;

Label::Label(const std::string &text) : font_("Tahoma"), text_(text) {
    text_item_ = new QGraphicsTextItem(this);
    draw();
}

QGraphicsItem *Label::get_graphics_item() { return this; }

void Label::set_font_size(int fontSize) {
    font_size_ = fontSize;
    draw();
}

void Label::set_font_color(const QColor &fontColor) {
    fontColor_ = fontColor;
    draw();
}

void Label::set_width(double width) {
    width_ = width;
    draw();
}

void Label::set_text(const std::string &text) {
    text_ = text;
    draw();
}

void Label::hoverEnterEvent(QGraphicsSceneHoverEvent *event) { emit hovered(); }

void Label::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { emit unhovered(); }

void Label::mousePressEvent(QGraphicsSceneMouseEvent *event) { emit clicked(this); }

void Label::draw() {
    text_item_->setPlainText(QString::fromStdString(text_));

    QFont font(font_.c_str(), font_size_);
    text_item_->setFont(font);
    text_item_->setDefaultTextColor(fontColor_);

    text_item_->setTextWidth(width_);
}

QRectF Label::boundingRect() const { return text_item_->boundingRect(); }
