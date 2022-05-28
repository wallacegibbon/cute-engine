#include "Map.h"
#include "Entity.h"
#include "EntitySprite.h"
#include "GUI.h"
#include "Game.h"
#include "PositionalSound.h"
#include "Sound.h"
#include "Sprite.h"
#include "TerrainLayer.h"
#include "Utilities.h"
#include "WeatherEffect.h"
#include "stl_helper.h"
#include <QDebug>

using namespace cute;

Map::Map(PathingMap *pathing_map) : own_pathing_map_(pathing_map) {
    /// a Map cannot be constructed with a null PathingMap
    assert(pathing_map != nullptr);

    num_cells_wide_ = pathing_map->num_cells_wide();
    num_cells_long_ = pathing_map->num_cells_long();
    cell_size_ = pathing_map->cell_size();

    terrain_layer_ = new QGraphicsRectItem();
    entity_layer_ = new QGraphicsRectItem();
    gui_layer_ = new QGraphicsRectItem();
    weather_layer_ = new QGraphicsRectItem();
    overall_pathing_map_ = new PathingMap(num_cells_wide_, num_cells_long_, cell_size_);
    scene_ = new QGraphicsScene(this);

    /// add layers in proper order
    scene_->addItem(terrain_layer_);
    scene_->addItem(entity_layer_);
    scene_->addItem(gui_layer_);
    scene_->addItem(weather_layer_);

    /// add a default TerrainLayer
    TerrainLayer *default_terrain = new TerrainLayer(width() / 256 + 1, height() / 256 + 1);
    // default_terrain->set_enable_fade(false);
    default_terrain->fill();
    add_terrain_layer(default_terrain);

    /// make background black
    QBrush bb;
    bb.setStyle(Qt::SolidPattern);
    bb.setColor(Qt::black);
    scene_->setBackgroundBrush(bb);
}

bool Map::contains(const QPointF &pos) {
    return pos.x() >= 0 && pos.y() >= 0 && pos.x() <= width() && pos.y() <= height();
}

bool Map::contains(Entity *entity) { return entities_.count(entity); }

QPointF Map::get_mouse_position() {
    if (game() == nullptr) {
        throw std::logic_error("Map isn't the current map.");
    }
    return game()->get_mouse_pos_in_map();
}

void Map::add_pathing_map(PathingMap &pm, const QPointF &at_pos) {
    stl_helper::add(additional_pathing_map_, &pm, at_pos);
}

void Map::remove_pathing_map(PathingMap &pm) { stl_helper::remove(additional_pathing_map_, &pm); }

/// merge each additional pathing map to own pathing map.
void Map::update_pathing_map() {
    delete overall_pathing_map_;
    overall_pathing_map_ = new PathingMap(num_cells_wide_, num_cells_long_, cell_size_);
    overall_pathing_map_->add_filling(*own_pathing_map_, QPointF(0, 0));

    for (auto &pm_pos : additional_pathing_map_) {
        overall_pathing_map_->add_filling(*pm_pos.first, pm_pos.second);
    }

    /// the following invocations are for debugging
    draw_pathing_map();
    draw_entity_pathing_map_bounds();
    draw_entity_bounding_boxes();
}

int Map::width() const { return own_pathing_map_->width(); }

int Map::height() const { return own_pathing_map_->height(); }

QSizeF Map::size() const { return QSizeF(width(), height()); }

QPointF Map::cell_to_point(const Node &cell) { return pathing_map().cell_to_point(cell); }

Node Map::point_to_cell(const QPointF &point) { return pathing_map().point_to_cell(point); }

void Map::remove_and_clear_debugging_polygons(std::vector<QGraphicsPolygonItem *> &polygons) {
    for (QGraphicsPolygonItem *p : polygons) {
        scene_->removeItem(p);
        delete p;
    }
    polygons.clear();
}

void Map::draw_pathing_map() {
    remove_and_clear_debugging_polygons(debugging_pathing_map_rects_);
    for (Node cell : pathing_map().cells()) {
        if (pathing_map().filled(cell)) {
            debugging_pathing_map_rects_.push_back(draw_filled_cell(cell));
        } else {
            debugging_pathing_map_rects_.push_back(draw_empty_cell(cell));
        }
    }
}

QGraphicsPolygonItem *Map::draw_empty_cell(Node &cell) {
    QGraphicsPolygonItem *rect =
            new QGraphicsPolygonItem(QRectF(0, 0, pathing_map().cell_size(), pathing_map().cell_size()));
    rect->setPos(cell.x() * pathing_map().cell_size(), cell.y() * pathing_map().cell_size());
    rect->setPen(QPen(Qt::red));
    scene_->addItem(rect);
    return rect;
}

QGraphicsPolygonItem *Map::draw_filled_cell(Node &cell) {
    QGraphicsPolygonItem *rect =
            new QGraphicsPolygonItem(QRectF(0, 0, pathing_map().cell_size(), pathing_map().cell_size()));
    rect->setPos(cell.x() * pathing_map().cell_size(), cell.y() * pathing_map().cell_size());
    rect->setBrush(QBrush(Qt::red));
    rect->setOpacity(0.25);
    scene_->addItem(rect);
    return rect;
}

void Map::draw_entity_bounding_boxes() {
    remove_and_clear_debugging_polygons(debugging_entity_bounding_boxes_);
    for (Entity *e : entities_) {
        debugging_entity_bounding_boxes_.push_back(scene_->addPolygon(e->map_to_map(e->bounding_rect())));
    }
}

void Map::draw_entity_pathing_map_bounds() {
    remove_and_clear_debugging_polygons(debugging_entity_pathing_map_boxes_);
    for (Entity *e : entities_) {
        QPointF topleft = e->map_to_map(e->pathing_map_pos());
        QGraphicsPolygonItem *poly = scene_->addPolygon(
                QRectF(topleft.x(), topleft.y(), e->pathing_map().width(), e->pathing_map().height()),
                QPen(Qt::yellow));
        debugging_entity_pathing_map_boxes_.push_back(poly);
    }
}

std::unordered_set<Entity *> Map::entities(const QRectF &rect) {
    QPolygonF region_as_poly(rect);
    std::unordered_set<Entity *> entities;
    for (Entity *entity : entities_) {
        QPolygonF entity_bounding_box = entity->map_to_map(entity->bounding_rect());
        if (!region_as_poly.intersected(entity_bounding_box).isEmpty()) {
            entities.insert(entity);
        }
    }
    return entities;
}

std::unordered_set<Entity *> Map::entities(const QPointF &at_point) {
    std::unordered_set<Entity *> entities;
    for (Entity *entity : entities_) {
        QPolygonF rect(entity->map_to_map(entity->bounding_rect()));
        if (rect.containsPoint(at_point, Qt::FillRule::OddEvenFill)) {
            entities.insert(entity);
        }
    }
    return entities;
}

std::unordered_set<Entity *> Map::entities(const QPolygonF &in_region) {
    std::unordered_set<Entity *> entities;
    for (Entity *entity : entities_) {
        QPolygonF entity_box = entity->map_to_map(entity->bounding_rect());
        if (!entity_box.intersected(in_region).isEmpty()) {
            entities.insert(entity);
        }
    }
    return entities;
}

std::unordered_set<Entity *> Map::entities(Entity *colliding_with) {
    QPolygonF bounding_box = colliding_with->map_to_map(colliding_with->bounding_rect());
    std::unordered_set<Entity *> results = entities(bounding_box);
    /// remove the entity itself
    results.erase(colliding_with);
    return results;
}

std::unordered_set<Entity *> Map::entities(const QRectF &in_region, double z_range_min, double z_range_max) {
    return filter_entities_by_z_range(entities(in_region), z_range_min, z_range_max);
}

std::unordered_set<Entity *> Map::entities(const QPointF &at_point, double z_range_min, double z_range_max) {
    return filter_entities_by_z_range(entities(at_point), z_range_min, z_range_max);
}

std::unordered_set<Entity *> Map::entities(const QPolygonF &in_region, double z_range_min, double z_range_max) {
    return filter_entities_by_z_range(entities(in_region), z_range_min, z_range_max);
}

std::unordered_set<Entity *> Map::filter_entities_by_z_range(std::unordered_set<Entity *> entities, double z_range_min,
                                                             double z_range_max) {
    std::unordered_set<Entity *> result;
    for (Entity *entity : entities) {
        double entitys_z = entity->z();
        double entitys_z_plus_height = entitys_z + entity->height();
        if ((entitys_z >= z_range_min && entitys_z <= z_range_max) ||
            (entitys_z_plus_height >= z_range_min && entitys_z_plus_height <= z_range_max)) {
            result.insert(entity);
        }
    }
    return result;
}

/// @warning Deletes the sprite after it is finished playing (in other words, Map takes ownership of the sprite).
void Map::play_once(Sprite *sprite, std::string animation, int delay_between_frames_ms, QPointF at_pos) {
    sprite->set_pos(at_pos);
    scene_->addItem(sprite);
    QObject::connect(sprite, &Sprite::animation_finished, sprite, &Sprite::deleteLater);
    sprite->play(animation, 1, 1 / ms_to_s(delay_between_frames_ms));
}

void Map::add_weather_effect(WeatherEffect &weather_effect) {
    /// TODO: handle case when weather effect is already in another map
    weather_effect.map_ = this;
    weather_effect.start();
    weather_effects_.insert(&weather_effect);
}

void Map::remove_weather_effect(WeatherEffect &weather_effect) {
    /// cannot remove a weather effect that has not been added
    assert(weather_effects_.count(&weather_effect) != 0);
    weather_effect.stop();
    weather_effect.map_ = nullptr;
    weather_effects_.erase(&weather_effect);
}

/// PositionalSounds adjust their volume based on their distance from the camera.
void Map::add_positional_sound(PositionalSound *sound) { sound->set_map_(this); }

/// Adds a picture ontop of the terrain at the specified position as a decoration.
void Map::add_terrain_decoration(const QPixmap &picture, const QPointF at_pos) {
    QGraphicsPixmapItem *pixmap_item = new QGraphicsPixmapItem(picture, terrain_layer_);
    pixmap_item->setPos(at_pos);
}

void Map::add_GUI(GUI *gui) { gui->get_graphics_item()->setParentItem(gui_layer_); }

void Map::remove_GUI(GUI *gui) {
    gui->get_graphics_item()->setParentItem(nullptr);
    scene_->removeItem(gui->get_graphics_item());
}

void Map::on_cam_moved(QPointF new_cam_pos) {
    if (game()->current_map() == this) {
        emit cam_moved(new_cam_pos);
    }
}

void Map::on_map_changed(Map *old_map, Map *new_map) {
    if (new_map == this) {
        emit set_as_current_map(this);
    }
    if (old_map == this && new_map != this) {
        emit unset_as_current_map(this);
    }
}

/// TerrainLayers stack in the order added (the later added, the "topper").
/// @lifetime The Map will uniquely own the lifetime of the specified TerrainLayer.
void Map::add_terrain_layer(TerrainLayer *terrain_layer) {
    terrain_layers_.push_back(terrain_layer);
    terrain_layer->parent_item_->setParentItem(terrain_layer_);
}

/// @lifetime You now uniquely own the lifetime of the removed terrain layer.
void Map::remove_terrain_layer(TerrainLayer *terrain_layer) {
    stl_helper::remove(terrain_layers_, terrain_layer);
    terrain_layer->parent_item_->setParentItem(nullptr);
    scene_->removeItem(terrain_layer->parent_item_);
}

/// If the Entity is in another Map, it will be removed from that Map first.
void Map::add_entity(Entity *entity) {
    if (contains(entity)) {
        qDebug() << "The entity " << entity << " is already in the map";
        return;
    }

    /// if the Entity is in another Map, remove it first
    Map *entitys_map = entity->map();
    if (entitys_map != nullptr) {
        entitys_map->remove_entity(entity);
    }

    /// add the entity to the list of entities
    entities_.insert(entity);

    /// add its sprite to the interal QGraphicsScene
    entity->sprite()->sprite_->setParentItem(entity_layer_);

    qreal bot = entity->map_to_map(entity->bounding_rect().bottomRight()).y();
    entity->sprite()->sprite_->set_z_value(bot);

    /// add its children's sprite's as a child of its sprites
    for (Entity *child : entity->children()) {
        child->set_parent_entity(entity);
    }

    /// update Entity's map_ ptr
    entity->map_ = this;

    /// update the PathingMap
    add_pathing_map(entity->pathing_map(), entity->map_to_map(entity->pathing_map_pos()));
    update_pathing_map();

    /// recursively add all child entities
    /// TODO: Why is this needed?
    for (Entity *child : entity->children()) {
        add_entity(child);
    }

    /// notify that this entity (and all of its child entities) have been added to the map.
    emit entity->map_entered(entity, this, entitys_map);
    emit entity_added(this, entity);
}

void Map::remove_entity(Entity *entity) {
    /// Entity not in map
    if (!contains(entity)) {
        return;
    }

    /// recursively remove its children
    for (Entity *child : entity->children()) {
        remove_entity(child);
    }

    /// remove from list
    entities_.erase(entity);

    /// remove sprite (if it has one)
    EntitySprite *entitys_sprite = entity->sprite();
    if (entitys_sprite != nullptr) {
        entitys_sprite->sprite_->setParentItem(nullptr);
        scene_->removeItem(entitys_sprite->sprite_);
    }

    /// set its internal pointer
    entity->map_ = nullptr;

    /// remove the pathing of the Entity
    remove_pathing_map(entity->pathing_map());
    update_pathing_map();

    /// emit entity left map event
    entity->map_left(entity, this);
    emit entity_removed(this, entity);
}

void Map::set_game(Game *game) {
    /// if the map has a previous game, stop listening to that games events
    if (game_) {
        disconnect(game_, &Game::cam_moved, this, &Map::on_cam_moved);
        disconnect(game_, &Game::map_changed, this, &Map::on_map_changed);
    }
    game_ = game;
    /// listen to new game
    if (game != nullptr) {
        connect(game_, &Game::cam_moved, this, &Map::on_cam_moved);
        connect(game, &Game::map_changed, this, &Map::on_map_changed);
    }
}
