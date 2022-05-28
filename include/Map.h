#pragma once

#include "Game.h"
#include "PathingMap.h"
#include "PositionalSound.h"
#include "TerrainLayer.h"
#include "Vendor.h"

class QPolygonF;
class QGraphicsItem;
class QGraphicsScene;

namespace cute {

class TerrainLayer;
class Entity;
class Sprite;
class WeatherEffect;

/// Represents a map which can contain a bunch of interacting Entities.
///
/// A Map has a PathingMap which keeps track of which cells are free and which are blocked.
/// Each Entity can also contain its own PathingMap therefore the Map is notified every time
/// an Entity is added/moved so that the Map's PathingMap can be updated accordingly.
///
/// A Map can be visualized by a Game. If the Map is currently being visualized by a game,
/// the Map::game() function will return that Game, otherwise it will return nullptr.

class Map : public QObject {
    Q_OBJECT

    /// game needs to be able to set game_ ptr of Map (in Game::set_current_map())
    friend class Game;

public:
    Map(PathingMap *pathing_map);

    Map(int num_cells_x, int num_cells_y, double cell_size)
            : Map(new PathingMap(num_cells_x, num_cells_y, cell_size)) {}

    Map() : Map(50, 50, 32) {}

    bool contains(const QPointF &pos);
    bool contains(Entity *entity);

    QPointF get_mouse_position();

    PathingMap &pathing_map() { return *overall_pathing_map_; }
    void add_pathing_map(PathingMap &pm, const QPointF &at_pos);
    void remove_pathing_map(PathingMap &pm);
    void update_pathing_map();

    int width() const;
    int height() const;
    QSizeF size() const;

    int num_cells_wide() const { return num_cells_wide_; }
    int num_cells_long() const { return num_cells_long_; }
    int cell_size() const { return cell_size_; }
    QPointF cell_to_point(const Node &cell);
    Node point_to_cell(const QPointF &point);

    std::unordered_set<Entity *> entities() { return entities_; }
    void add_entity(Entity *entity);
    void remove_entity(Entity *entity);

    void add_terrain_layer(TerrainLayer *terrain_layer);
    void remove_terrain_layer(TerrainLayer *terrain_layer);
    std::vector<TerrainLayer *> terrain_layers() { return terrain_layers_; }

    QGraphicsScene *scene() { return scene_; }

    /// If the map isn't currently being visualized, returns nullptr.
    Game *game() { return game_; }

    /// for testing
    void remove_and_clear_debugging_polygons(std::vector<QGraphicsPolygonItem *> &polygons);
    void draw_pathing_map();
    QGraphicsPolygonItem *draw_filled_cell(Node &cell);
    QGraphicsPolygonItem *draw_empty_cell(Node &cell);
    void draw_entity_pathing_map_bounds();
    void draw_entity_bounding_boxes();

    /// getting all entities at a certain point/region/colliding w other entities
    std::unordered_set<Entity *> entities(const QRectF &rect);
    std::unordered_set<Entity *> entities(const QPointF &at_point);
    std::unordered_set<Entity *> entities(const QPolygonF &in_region);
    std::unordered_set<Entity *> entities(Entity *colliding_with);
    std::unordered_set<Entity *> entities(const QRectF &in_region, double z_range_min, double z_range_max);
    std::unordered_set<Entity *> entities(const QPointF &at_point, double z_range_min, double z_range_max);
    std::unordered_set<Entity *> entities(const QPolygonF &in_region, double z_range_min, double z_range_max);

    std::unordered_set<Entity *> filter_entities_by_z_range(std::unordered_set<Entity *> entities, double z_range_min,
                                                            double z_range_max);

    void play_once(Sprite *sprite, std::string animation, int delay_between_frames_ms, QPointF at_pos);

    void add_weather_effect(WeatherEffect &weather_effect);
    void remove_weather_effect(WeatherEffect &weather_effect);

    void add_positional_sound(PositionalSound *sound);

    void add_terrain_decoration(const QPixmap &picture, const QPointF at_pos);

    void add_GUI(GUI *gui);
    void remove_GUI(GUI *gui);

signals:
    void set_as_current_map(Map *sender);
    void unset_as_current_map(Map *sender);

    /// This is only emitted when this map is set as the current Map for the Game.
    void cam_moved(QPointF new_cam_pos);

    void entity_added(Map *sender, Entity *entity);
    void entity_removed(Map *sender, Entity *entity);
    void entity_moved(Map *sender, Entity *entity);

public slots:
    void on_cam_moved(QPointF new_cam_pos);
    void on_map_changed(Map *old_map, Map *new_map);

private:
    void set_game(Game *game);

private:
    int num_cells_wide_;
    int num_cells_long_;
    int cell_size_;
    int width_;
    int height_;

    /// Map's own pathing map, this can be arbitrarly filled by client
    PathingMap *own_pathing_map_;

    /// additional pathing maps (and their location)
    std::unordered_map<PathingMap *, QPointF> additional_pathing_map_;

    /// the "merge" of "own_pathing_map_" and all "additional_pathing_map_"
    /// overall_pathing_map_ has the same size as own_pathing_map_
    PathingMap *overall_pathing_map_;

    std::unordered_set<Entity *> entities_;
    std::vector<TerrainLayer *> terrain_layers_;
    std::set<WeatherEffect *> weather_effects_;

    /// the game that is currently visualizing the map, null if not currently being visualizing
    QPointer<Game> game_ = nullptr;

    QGraphicsRectItem *terrain_layer_;
    QGraphicsRectItem *entity_layer_;
    QGraphicsRectItem *gui_layer_;
    QGraphicsRectItem *weather_layer_;

    /// the Map itself is basicly a wrapper of QGraphicsScene
    QGraphicsScene *scene_;

    /// for debugging
    std::vector<QGraphicsPolygonItem *> debugging_pathing_map_rects_;
    std::vector<QGraphicsPolygonItem *> debugging_entity_pathing_map_boxes_;
    std::vector<QGraphicsPolygonItem *> debugging_entity_bounding_boxes_;
};

} // namespace cute
