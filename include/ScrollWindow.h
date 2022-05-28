#pragma once

#include "GUI.h"
#include "ScrollBar.h"
#include "Vendor.h"

namespace cute {

class Panel;

/// Represents a GUI that can be used to visualized other GUIs in a scroll like
/// fashion. You can add GUI's to this GUI and then use the scroll bars to
/// view the different GUI's as you scroll...I hope that makes sense :),
/// if not, try it out :)
/// @author Abdullah Aghazadah
/// @date 10-9-16
class ScrollWindow : public GUI {
    Q_OBJECT
public:
    ScrollWindow();
    ScrollWindow(double width, double height);

    void set_height(double height);
    void set_width(double width);
    double height() { return height_; }
    double width() { return width_; }
    void show_border(bool tf);
    void set_border_color(const QColor &color);
    void set_border_thickness(double thickness);
    void show_background(bool tf);
    void set_background_color(const QColor &color);
    void set_background_pixmap(const QPixmap &pixmap);

    void add(GUI *gui, QPointF at_pos);
    void remove(GUI *gui);
    void remove_all();
    bool contains(GUI *gui);
    std::unordered_set<GUI *> added_GUIs();

    QGraphicsItem *get_graphics_item() override;

public slots:
    void any_position_changed(double pos);

private:
    void draw();

private:
    double width_;
    double height_;

    std::unordered_map<GUI *, QPointF> GUI_to_pos_;
    std::unordered_map<GUI *, QPointF> GUI_in_view_to_shift_vector_;

    std::unique_ptr<ScrollBar> vertical_scroll_bar_;
    ScrollBar *horizontal_scroll_bar_;
    Panel *background_;
};

} // namespace cute
