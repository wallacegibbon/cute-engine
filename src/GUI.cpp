#include "GUI.h"
#include "Game.h"

using namespace cute;

GUI::~GUI() {
    for (GUI *gui : child_GUIs_) {
        delete gui;
    }
}

QPointF GUI::GUI_pos() { return get_graphics_item()->pos(); }

void GUI::set_GUI_pos(const QPointF &pos) { get_graphics_item()->setPos(pos); }

void GUI::set_GUI_rotation(double degrees) { get_graphics_item()->setRotation(degrees); }

/// If nullptr is passed in, removes current parent.
void GUI::set_parent_GUI(GUI *gui) {
    /// approach:
    /// - set parent of internal graphics item and positioning/scene removal is handled automatically

    /// if gui is null but parent is already null, don't do anything
    if (gui == nullptr && parent_GUI_ == nullptr) {
        return;
    }
    /// if gui is null but it has a parent, remove the parent
    if (gui == nullptr && parent_GUI_ != nullptr) {
        parent_GUI_->child_GUIs_.erase(this);
        get_graphics_item()->setParentItem(nullptr);
        get_graphics_item()->setVisible(false);
        parent_GUI_ = nullptr;
        return;
    }

    /// set parent to an actual gui
    get_graphics_item()->setParentItem(gui->get_graphics_item());
    get_graphics_item()->setVisible(true);
    gui->child_GUIs_.insert(this);
    parent_GUI_ = gui;
}

Game *GUI::game() { return game_; }

QRectF GUI::get_GUI_bounding_box() {
    /// approach:
    /// - get graphics item, then recursively find all bboxes
    /// - find an encompassing bounding box

    std::vector<QRectF> bboxes = get_bounding_boxes_for(get_graphics_item(), get_graphics_item());

    QRectF initial_rect = bboxes[0];
    double lowest_y = initial_rect.top();
    double highest_y = initial_rect.bottom();
    double lowest_x = initial_rect.left();
    double highest_x = initial_rect.right();

    for (QRectF rect : bboxes) {
        if (rect.top() < lowest_y) {
            lowest_y = rect.top();
        }
        if (rect.bottom() > highest_y) {
            highest_y = rect.bottom();
        }
        if (rect.left() < lowest_x) {
            lowest_x = rect.left();
        }
        if (rect.right() > highest_x) {
            highest_x = rect.right();
        }
    }

    QRectF total_box(QPointF(lowest_x, lowest_y), QPointF(highest_x, highest_y));
    total_box.moveTopLeft(QPointF(0, 0));

    return total_box;
}

std::vector<QRectF> GUI::get_bounding_boxes_for(QGraphicsItem *gi, QGraphicsItem *map_to) {
    /// push gi bounding rect
    std::vector<QRectF> boxes;
    QRectF bounding_box = gi->boundingRect();
    bounding_box.moveTopLeft(gi->mapToItem(map_to, QPointF(0, 0)));
    boxes.push_back(bounding_box);

    /// push all childrens bounding rect
    for (QGraphicsItem *child : gi->childItems()) {
        for (QRectF rect : get_bounding_boxes_for(child, map_to)) {
            boxes.push_back(rect);
        }
    }

    return boxes;
}
