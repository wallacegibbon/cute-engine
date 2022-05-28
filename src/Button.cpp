#include "Button.h"

using namespace cute;

Button::Button()
        : text_("default text"), background_pixmap_(":/cute-engine-builtin/resources/graphics/misc/buttonbg.png") {
    on_click_callback_ = [](Button *button) {};
    draw();
}

QGraphicsItem *Button::get_graphics_item() { return this; }

void Button::set_border_padding(double amount) {
    border_padding_ = amount;
    draw();
}

void Button::set_font_size(int size) {
    font_size_ = size;
    draw();
}

void Button::set_text(const std::string &text) {
    text_ = text;
    draw();
}

void Button::set_text_color(const QColor &color) {
    text_color_ = color;
    draw();
}

void Button::set_text_bold(bool tf) {
    is_text_bold_ = tf;
    draw();
}

void Button::set_text_italic(bool tf) {
    is_text_italic_ = tf;
    draw();
}

void Button::set_background_color(const QColor &color) {
    background_color_ = color;
    is_background_pixmap_ = false;
    draw();
}

void Button::set_background_pixmap(const QPixmap &pixmap) {
    background_pixmap_ = pixmap;
    is_background_pixmap_ = true;
    draw();
}

double Button::height() { return background_pixmap_.height(); }

double Button::width() { return background_pixmap_.width(); }

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit clicked();
    on_click_callback_(this);
}

void Button::draw() {
    /// draw text
    if (text_item_) {
        delete text_item_;
    }

    text_item_ = new QGraphicsTextItem(QString::fromStdString(text_), this);
    text_item_->setDefaultTextColor(text_color_);
    text_item_->setPos(border_padding_, border_padding_);

    QFont font("Times", font_size_);
    font.setBold(is_text_bold_);
    font.setItalic(is_text_italic_);
    text_item_->setFont(font);

    /// draw background
    double fontWidth = text_item_->boundingRect().width();
    double fontHeight = text_item_->boundingRect().height();
    double bg_width = fontWidth + border_padding_ * 2;
    double bg_height = fontHeight + border_padding_ * 2;

    if (is_background_pixmap_) {
        background_pixmap_ = background_pixmap_.scaled(bg_width, bg_height);
        setPixmap(background_pixmap_);
    } else {
        QImage img(QSize(bg_width, bg_height), QImage::Format_RGB32);
        img.fill(background_color_);
        setPixmap(QPixmap::fromImage(img));
    }
}
