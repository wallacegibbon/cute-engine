#pragma once

#include "DiplomacyManager.h"
#include "Vendor.h"

namespace cute {

class Map;
class MapGrid;
class Entity;
class InventoryViewer;
class Entity;
class GUI;

/// This is basically the window the will visualize a Map.
/// This class is a singleton, thus you can only construct one instance.
/// The Game::game pointer is global, you can access it from anywhere.
class Game : public QGraphicsView {
    Q_OBJECT

public:
    /// clang-format off
    enum class MouseMode { Regular, SelectPosition, SelectEntity };
    /// clang-format on

    static Game *game;

    Game(MapGrid *map_grid, int x_pos_of_starting, int y_pos_of_starting);

    void launch();

    MapGrid *map_grid() { return map_grid_; }
    void set_current_map(Map *current_map);
    Map *current_map() { return current_map_; }

    QPointF map_to_map(const QPoint &point);
    QPointF map_from_map(const QPointF &point);

    /// camera controls
    void set_center_cam_pos(QPointF position);
    QPointF center_cam_pos();
    QRectF cam();
    void move_cam(QVector2D by_vector);
    void move_cam_up(double by_amount);
    void move_cam_down(double by_amount);
    void move_cam_left(double by_amount);
    void move_cam_right(double by_amount);

    /// mouse/key event handlers
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void add_GUI(GUI *gui);
    void remove_GUI(GUI *gui);
    bool contains_GUI(GUI *gui);

    std::set<int> keys_pressed() { return key_pressed_; }
    QPoint get_mouse_pos();
    QPointF get_mouse_pos_in_map();

    MouseMode mouse_mode() { return mouse_mode_; }
    void set_mouse_mode(MouseMode mode);

    /// TODO delete test
    std::vector<Entity *> enemies_;

    void add_watched_entity(Entity *watched, Entity *watching, double range);
    bool watched_watching_pair_exists(Entity *watched, Entity *watching);
    void remove_watched_entity(Entity *watched, Entity *watching);
    void remove_watched_entity(Entity *watched);
    void remove_watching_entity(Entity *watching);
    std::set<Entity *> watched_entities();
    std::set<Entity *> watched_entities(Entity *watching_entity);
    std::set<Entity *> watching_entities();
    std::set<Entity *> watching_entities(Entity *watched_entity);
    double watched_watching_range(Entity *watched, Entity *watching);
    void set_watched_watching_range(Entity *watched, Entity *watching, double range);

    DiplomacyManager &diplomacy_manager();

signals:
    void position_selected(QPointF pos);
    void entity_selected(Entity *entity);
    void select_canceled();

    /// Emitted whenever the a mouse button is pressed down while Game is in Regular MouseMode.
    /// @param pos The position selected, in screen coordinates.
    /// @param button The mouse button that was pressed.
    void mouse_pressed(QPoint pos, Qt::MouseButton button);

    /// Emitted whenever the mouse is moved while the Game is in Regular MouseMode.
    void mouse_moved(QPoint pos);

    /// Emitted whenever a mouse button is released while the Game is in Regular MouseMode.
    void mouse_released(QPoint pos, Qt::MouseButton button);

    void key_pressed(int key);
    void key_released(int key);

    void map_changed(Map *old_map, Map *new_map);

    /// For example, suppose you have entity A and B. You want A to be notified
    /// whenever B enters within 100 pixels of it. You can do this by calling
    /// add_watched_entity(B, A, 100);
    void watched_entity_enters_range(Entity *watched, Entity *watching, double range);
    void watched_entity_leaves_range(Entity *watched, Entity *watching, double range);

    /// new_pos is the new position of the camera (its center).
    void cam_moved(QPointF new_pos);

public slots:
    void update_GUI_positions();
    void on_watched_entity_dies(QObject *watched_entity);
    void on_watching_entity_dies(QObject *watching_entity);

    /// currently, the following function is called directly, not by signal-slot mechanism.
    void on_entity_moved(Entity *entity);

private:
    bool handle_watched_watching_pair(Entity *watched_entity, Entity *watching_entity);
    bool is_key_event_to_ignore(QKeyEvent *event);

private:
    MapGrid *map_grid_;
    Map *current_map_;
    std::set<int> key_pressed_;
    MouseMode mouse_mode_;

    QTimer *gui_update_timer_;
    std::unordered_set<GUI *> gui_s_;

    /// the global gui layer which will be put into active map
    QGraphicsRectItem *gui_layer_;

    DiplomacyManager diplomacy_manager_;

    /// the relation-graph of watched & watching entities
    std::map<Entity *, std::set<Entity *>> watched_to_watching_;
    std::map<Entity *, std::set<Entity *>> watching_to_watched_;

    /// for each watched/watching pair, what is the range?
    std::map<std::pair<Entity *, Entity *>, double> watched_watching_pair_to_range_;

    /// has the enters range event been emitted yet for a pair of watched watching entities?
    std::map<std::pair<Entity *, Entity *>, bool> watched_watching_pair_to_enter_range_emitted_;
};

} // namespace cute
