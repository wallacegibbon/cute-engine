#include "ScrollWindow.h"
#include "Panel.h"
#include "ScrollBar.h"

using namespace cute;

ScrollWindow::ScrollWindow() : ScrollWindow(300, 300) {}

ScrollWindow::ScrollWindow(double width, double height)
        : width_(width), height_(height), vertical_scroll_bar_(new ScrollBar()) {

    horizontal_scroll_bar_ = new ScrollBar();
    horizontal_scroll_bar_->set_parent_GUI(vertical_scroll_bar_.get());

    background_ = new Panel();
    background_->set_parent_GUI(vertical_scroll_bar_.get());

    connect(vertical_scroll_bar_.get(), &ScrollBar::position_changed, this, &ScrollWindow::any_position_changed);
    connect(horizontal_scroll_bar_, &ScrollBar::position_changed, this, &ScrollWindow::any_position_changed);

    draw();
}

void ScrollWindow::add(GUI *gui, QPointF at_pos) {
    if (contains(gui)) {
        return;
    }

    gui->set_parent_GUI(background_);
    GUI_to_pos_[gui] = at_pos;
    draw();
}

void ScrollWindow::remove(GUI *gui) {
    assert(contains(gui));

    gui->set_parent_GUI(nullptr);
    GUI_to_pos_.erase(gui);
    draw();
}

void ScrollWindow::remove_all() {
    /// set parent of all guis to nullptr
    for (std::pair<GUI *, QPointF> GUI_point : GUI_to_pos_) {
        GUI *gui = GUI_point.first;
        gui->set_parent_GUI(nullptr);
    }
    /// clear guis
    GUI_to_pos_.clear();
    draw();
}

bool ScrollWindow::contains(GUI *gui) { return GUI_to_pos_.find(gui) != GUI_to_pos_.end(); }

void ScrollWindow::set_height(double height) {
    height_ = height;
    draw();
}

void ScrollWindow::set_width(double width) {
    width_ = width;
    draw();
}

void ScrollWindow::show_border(bool tf) { background_->show_border(tf); }

void ScrollWindow::set_border_color(const QColor &color) { background_->set_border_color(color); }

void ScrollWindow::set_border_thickness(double thickness) { background_->set_border_thickness(thickness); }

void ScrollWindow::show_background(bool tf) { background_->show_background(tf); }

void ScrollWindow::set_background_color(const QColor &color) { background_->set_background_color(color); }

void ScrollWindow::set_background_pixmap(const QPixmap &pixmap) { background_->set_background_pixmap(pixmap); }

QGraphicsItem *ScrollWindow::get_graphics_item() { return vertical_scroll_bar_.get(); }

void ScrollWindow::any_position_changed(double pos) { draw(); }

/// approach:
/// - look at position of the vertical and horizontal scroll bar and
///  determine the "view bounding box"
/// - traverse through guis, see which ones are in the view bounding box
/// - for the ones in the view bounding box, remember their shift vector
/// relative to top left of view bounding box
/// - for the guis in view bounding box, set their visibility to true
///  (all others to false initially), and draw them relative to this gui, but
/// with shift vector applied
void ScrollWindow::draw() {
    /// draw the view border/background
    double window_x = vertical_scroll_bar_->bg_bar_width();
    double window_y = 0;
    background_->set_GUI_pos(QPointF(window_x, window_y));
    background_->set_width(width_);
    background_->set_height(height_);

    /// set all guis invisible
    for (std::pair<GUI *, QPointF> GUI_point : GUI_to_pos_) {
        GUI *gui = GUI_point.first;
        gui->get_graphics_item()->setVisible(false);
    }

    /// calculate total height and width
    double total_bottom = 0;
    double total_right = 0;
    for (std::pair<GUI *, QPointF> GUI_point : GUI_to_pos_) {
        GUI *gui = GUI_point.first;
        QPointF GUI_pos = GUI_point.second;
        double bottom = GUI_pos.y() + gui->get_GUI_bounding_box().height();
        double right = GUI_pos.x() + gui->get_GUI_bounding_box().width();

        if (bottom > total_bottom) {
            total_bottom = bottom;
        }
        if (right > total_right) {
            total_right = right;
        }
    }
    double total_height = total_bottom;
    double total_width = total_right;
    if (total_height < height_) {
        total_height = height_ + 1;
    }
    if (total_width < width_) {
        total_width = width_ + 1;
    }
    /// draw scroll bars properly
    vertical_scroll_bar_->set_bg_bar_length(height_);
    horizontal_scroll_bar_->set_bg_bar_length(width_);
    horizontal_scroll_bar_->set_GUI_pos(QPointF(20, height_ + 20));
    horizontal_scroll_bar_->setRotation(-90);
    double vertical_fraction = height_ / total_height;
    double horizontal_fraction = width_ / total_width;
    vertical_scroll_bar_->set_fg_bar_length_as_fraction_of_bg_bar_length(vertical_fraction);
    horizontal_scroll_bar_->set_fg_bar_length_as_fraction_of_bg_bar_length(horizontal_fraction);

    /// find view bounding box
    double bbox_top = vertical_scroll_bar_->fg_bar_top_pos() * total_height;
    double bbox_bottom = vertical_scroll_bar_->fg_bar_bottom_pos() * total_height;
    double bbox_left = horizontal_scroll_bar_->fg_bar_top_pos() * total_width;
    double bbox_right = horizontal_scroll_bar_->fg_bar_bottom_pos() * total_width;
    QRectF view_bounding_box(QPointF(bbox_left, bbox_top), QPointF(bbox_right, bbox_bottom));

    /// find out which guis are in the view bounding box
    GUI_in_view_to_shift_vector_.clear();
    for (std::pair<GUI *, QPointF> GUI_point : GUI_to_pos_) {
        GUI *gui = GUI_point.first;
        QPointF GUI_pos = GUI_point.second;
        QRectF GUI_bounding_box = gui->get_GUI_bounding_box();
        GUI_bounding_box.moveTopLeft(GUI_pos);

        if (view_bounding_box.contains(GUI_bounding_box)) {
            QPointF shift_vector(GUI_pos.x() - view_bounding_box.x(), GUI_pos.y() - view_bounding_box.y());
            GUI_in_view_to_shift_vector_[gui] = shift_vector;
        }
    }

    for (std::pair<GUI *, QPointF> GUI_point : GUI_in_view_to_shift_vector_) {
        GUI *gui = GUI_point.first;
        gui->get_graphics_item()->setVisible(true);
        gui->set_GUI_pos(GUI_in_view_to_shift_vector_[gui]);
    }
}
