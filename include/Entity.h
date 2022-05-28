#pragma once

#include "Map.h"
#include "PathingMap.h"
#include "Vendor.h"

class QPointF;
class QTimer;

namespace cute {

class Map;
class Inventory;
class EquipableItem;
class Item;
class EntitySprite;
class Slot;
class Inventory;
class EntityController;
class PositionalSound;

/// When an Entity is first added to a Map, the map will use the Entitie's
/// PathingMap and position to update its own PathingMap.
/// When an Entity is moved, it will notify the Map so that it can once again update the PathingMap.
/// When an entity is removed from the Map, the Map's pathing map will be updated.

class Entity : public QObject {
    Q_OBJECT

    /// Map needs to be able to set the map_ pointer of the Entity
    /// (we cannot use a public setter because we don't want ANYONE else to be able to set this)
    friend class Map;

public:
    Entity();
    virtual ~Entity();

    PathingMap &pathing_map() const { return *pathing_map_; }
    void set_pathing_map(PathingMap &pathing_map, const QPointF &pos = QPointF(0, 0));
    void set_pathing_map_pos(const QPointF &to) { pathing_map_pos_ = to; }

    /// map b/w Entity local coor system and Map coor system
    Map *map() const { return map_; }
    QPointF map_to_map(const QPointF &point) const;
    QPolygonF map_to_map(const QRectF &rect) const;

    QPointF pos() const { return current_pos_; }

    double x() const { return pos().x(); }
    double y() const { return pos().y(); }
    double z() const { return z_pos_; }

    double height() const { return height_; }

    QPointF top_left() const { return current_pos_ - origin(); }
    QPointF bot_right() const { return current_pos_ + origin(); }

    void set_pos(const QPointF &pos);
    void set_pos(std::string named_pos, const QPointF &pos);
    void set_pos(const QPointF &entitys_point, const QPointF &to_pos);
    void set_x(double x);
    void set_y(double y);
    void set_z(double z);

    void move_by(double dx, double dy) { set_pos(QPointF(x() + dx, y() + dy)); }

    void set_height(double height) { height_ = height; }

    Node cell_pos() { return map()->pathing_map().point_to_cell(pos()); }

    void set_cell_pos(const Node &cell);
    QPointF pathing_map_pos() const { return pathing_map_pos_; }

    int facing_angle() { return facing_angle_; }
    void set_facing_angle(double angle);

    void face_point(const QPointF &point);

    QPointF origin() const { return origin_; }
    void set_origin(const QPointF &p) { origin_ = p; }

    virtual void set_bounding_rect(const QRectF &rect) { bounding_rect_ = rect; }
    virtual QRectF bounding_rect() const { return bounding_rect_; }

    QPolygonF bounding_polygon_in_map() const { return map_to_map(bounding_rect()); }

    void set_sprite(EntitySprite *sprite, bool auto_set_origin_and_bounding_box = true);
    EntitySprite *sprite() const { return sprite_; }

    void set_bounding_box_and_update_origin(const QRectF &rect);

    std::unordered_set<Entity *> children() const { return children_; }
    void set_parent_entity(Entity *parent);
    Entity *parent() { return parent_; }
    bool has_child(Entity *entity) const { return children_.find(entity) != children_.end(); }
    bool has_child_recursive(Entity *entity) const;

    void add_named_point(const QPointF &point, std::string name) { named_points_[name] = point; }
    QPointF named_point(std::string name);

    void add_tag(const std::string &tag);
    bool contains_tag(const std::string &tag);

    void set_speed(double speed) { speed_ = speed; }
    double speed() { return speed_; }

    void set_rotation_speed(double rotation_speed) { rotation_speed_ = rotation_speed; }
    double rotation_speed() { return rotation_speed_; }

    void set_health(double health);
    double health() { return health_; }
    void set_max_health(double max_health) { max_health_ = max_health; }
    double max_health() { return max_health_; }

    void damage_enemy(Entity *entity, double amount) const;
    void damage_enemy_and_neutral(Entity *entity, double amount) const;
    void damage_anyone_except_children(Entity *entity, double amount) const;
    void damage_entity(Entity *entity, double amount) const;

    bool invulnerable() { return invulnerable_; }
    void set_invulnerable(bool tf) { invulnerable_ = tf; }

    void set_group(int group_number) { group_number_ = group_number; }
    int group() const { return group_number_; }
    Relationship relationship_towards(const Entity &entity) const;

    void add_slot(Slot *slot, const std::string &name);
    Slot *slot(std::string name);
    std::unordered_set<Slot *> get_slots();

    bool equip_item(EquipableItem *item, std::string slot);
    bool equip_item(EquipableItem *item, Slot *slot);

    void set_inventory(Inventory *inv) { inventory_ = inv; }
    Inventory *inventory() { return inventory_; }

    bool can_fit(const QPointF &at_pos);

    void add_sound(const std::string &sound_name, const std::string &file_path);
    void play_sound(const std::string &sound_name, int num_times_to_play);

public slots:
    void check_die(EntitySprite *sender, std::string animation);

signals:
    void moved(Entity *sender, QPointF from_pos, QPointF to_pos);
    void want_to_move_but_cannot(Entity *sender, QPointF from_pos, QPointF to_pos);

    void collided(Entity *sender, Entity *collided_with);

    void map_entered(Entity *sender, Map *map_entered, Map *map_old);
    void map_left(Entity *sender, Map *map_just_left);

    void health_changed(Entity *sender);

    void dying(Entity *sender);

private:
    void scale_based_on_z();

private:
    PathingMap *pathing_map_;

    /// The position that the PathingMap is placed relative to the Entity
    QPointF pathing_map_pos_;

    /// The position is relative to the parent Entity.
    /// If there is no parent Entitiy, it is relative to the Map.
    QPointF current_pos_;

    /// location of the sprite that is the considered the "origin" of this entity
    /// When the position of the Entity is set, this is the point of the sprite that is moved to the specified position.
    /// When the Entity rotates, this is the point about which the sprite will rotate.
    QPointF origin_;

    /// The bounding rect is the area of the entity's sprite that is considered to be it's "solid" part.
    /// This is the area that will be used to test for collisions and such.
    QRectF bounding_rect_;
    double z_pos_ = 0;
    double height_ = 0;

    double facing_angle_ = 0;

    /// The speed that the entity should generally travel at (in pixels per second).
    /// Note that entity controllers often use this variable to determine how to move the entity,
    /// so this speed may not be EXACTLY what the actual movement speed of the entity ends up being.
    double speed_ = 250;

    /// The speed at which this entity should in general rotate at, in degrees per second.
    /// Note that entity controllers often use this variable to determine how to rotate the entity,
    /// so this rotation speed may not be exactly what the actual ends up being.
    /// (that is up to the entity controllers).
    double rotation_speed_ = 360;

    Map *map_ = nullptr;
    EntitySprite *sprite_;

    std::unordered_set<Entity *> children_;
    Entity *parent_ = nullptr;

    /// (the x and y of) these points are relative to this entity's top-left point
    std::map<std::string, QPointF> named_points_;

    std::set<std::string> tags_;

    double health_ = 10;
    double max_health_ = 100;
    int group_number_ = 0;
    bool invulnerable_ = false;

    Inventory *inventory_;
    std::unordered_map<std::string, Slot *> string_to_slot_;

    QPointF last_pos_;

    std::unordered_map<std::string, std::string> sound_name_to_filepath_;
    std::unordered_map<std::string, PositionalSound *> sound_path_to_positional_;
};

} // namespace cute

namespace std {

/// make QPointer<Entity> hashable (simply "delegate" to hasing a regular Entity*)
template <>
struct hash<QPointer<cute::Entity>> {
    size_t operator()(const QPointer<cute::Entity> &node) const { return hash<cute::Entity *>()(node.data()); }
};

/// hash pair of entities (order shouldn't matter)
template <>
struct hash<std::pair<cute::Entity *, cute::Entity *>> {
    size_t operator()(const std::pair<cute::Entity *, cute::Entity *> &pair_of_entities) const {
        return (53 + hash<cute::Entity *>()(pair_of_entities.first)) * 53 +
               hash<cute::Entity *>()(pair_of_entities.second);
    }
};

} // namespace std
