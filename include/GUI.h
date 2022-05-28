#pragma once

#include "Vendor.h"

class QGraphicsItem;
class QGraphicsItem;
class QRectF;

namespace cute {

class Game;

/// Abstract class that Represents a GUI element that can be added to a Game or a Map.
///
/// A GUI is placed relative to its parent GUI, if it does not have a parent
/// GUI, its place relative to the top left hand corner of the Game's screen
/// (if added to Game) or to top left hand corner of the Map (if added to Map).
///
/// Most GUIs offer functions for customizing their look in some way.
/// Most GUIs emit signals when they are interacted with.
/// When a parent GUI is deleted, it will delete all of its child GUIs.

class GUI : public QObject {
    Q_OBJECT

    /// game needs to access GUI::game_ variable (needs to set it during Game::add_GUI/remove_GUI)
    friend class Game;

public:
    ~GUI();

    QPointF GUI_pos();
    void set_GUI_pos(const QPointF &guiPos);
    void set_GUI_rotation(double degrees);

    void set_parent_GUI(GUI *gui);

    Game *game();

    /// Returns the root QGraphicsItem that has the drawings for the GUI.
    /// This is the QGraphicsItem that goes in the scene when Game::add_GUI() is called.
    virtual QGraphicsItem *get_graphics_item() = 0;

    /// Returns the bounding box for the GUI.
    /// The default implementation will return a bounding box that encompasses all the child GUI's bounding box.
    /// This function is virtual, so that sub classes may provide a better implementation if they wish.
    virtual QRectF get_GUI_bounding_box();

private:
    std::vector<QRectF> get_bounding_boxes_for(QGraphicsItem *gi, QGraphicsItem *map_to);

private:
    GUI *parent_GUI_ = nullptr;
    QPointer<Game> game_;
    std::unordered_set<GUI *> child_GUIs_;
};

} // namespace cute
