#pragma once

#include "GUI.h"
#include "Vendor.h"

class QGraphicsTextItem;

namespace cute {

// TODO: reimplement using Panel as background (reduces duplicate code)

/// Represents a button GUI that can be added to a Game. You can set various
/// options such as the text color, padding b/w the text and the button's edges,
/// weather the text is bold/italic, etc...
/// When the Button is clicked, it will emit a clicked() signal.
class Button : public GUI, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    Button();

    QGraphicsItem *get_graphics_item() override;

    void set_border_padding(double amount);
    void set_font_size(int size);
    void set_text(const std::string &text);
    void set_text_color(const QColor &color);
    void set_text_bold(bool tf);
    void set_text_italic(bool tf);
    void set_background_color(const QColor &color);
    void set_background_pixmap(const QPixmap &pixmap);

    double height();
    double width();

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void set_on_click_callback(std::function<void(Button *)> cb) { on_click_callback_ = cb; }

signals:
    void clicked();

private:
    void draw();

private:
    QGraphicsTextItem *text_item_ = nullptr;
    double border_padding_ = 4;
    QColor text_color_ = Qt::cyan;
    QColor background_color_ = Qt::darkGray;
    QPixmap background_pixmap_;
    bool is_background_pixmap_ = true;
    bool is_text_bold_ = false;
    bool is_text_italic_ = false;
    int font_size_ = 12;
    std::string text_;

    /// a function that is called whenever the Button is clicked
    /// (in addition to the "clicked" signal being emitted)
    std::function<void(Button *)> on_click_callback_;
};

} // namespace cute
