#include "Entity.h"
#include "EntitySprite.h"
#include "EquipableItem.h"
#include "Game.h"
#include "Inventory.h"
#include "Map.h"
#include "Slot.h"
#include "Sprite.h"
#include "TopDownSprite.h"
#include "Utilities.h"
#include "stl_helper.h"

using namespace cute;

Entity::Entity() : children_() {
    /// the size of an entity's sprite is usually 128x128
    pathing_map_ = new PathingMap(4, 4, 32);
    // pathing_map_->fill(QRectF(QPointF(0, 0), QPointF(128, 128)));
    pathing_map_pos_ = QPointF(0, 0);
    sprite_ = new TopDownSprite();
    sprite_->setParent(this);
    inventory_ = new Inventory(this);
}

Entity::~Entity() {
    /// let listeners know this entity is *about* to die
    emit dying(this);

    /// recursively delete child Entities
    for (Entity *entity : children()) {
        delete entity;
    }
    /// if the entity has a parent, remove from parents list of children
    Entity *parent_entity = parent();
    if (parent_entity != nullptr) {
        parent_entity->children_.erase(this);
    }

    /// if the entity is in a map, remove it
    if (map_ != nullptr) {
        map_->remove_entity(this);
    }
}

/// The old PathingMap is removed from the entity's Map, however it is not deleted.
/// Client is responsible for lifetime of old map.
/// @param pos The pos that the PathingMap is placed relative to the Entity.
void Entity::set_pathing_map(PathingMap &pathing_map, const QPointF &pos) {
    PathingMap *old_pathing_map = pathing_map_;
    pathing_map_ = &pathing_map;
    pathing_map_pos_ = pos;
    if (map_ != nullptr) {
        map_->remove_pathing_map(*old_pathing_map);
    }
}

/// The position is relative to the parent Entity. If there is no parent Entitiy, it is relative to the Map.
void Entity::set_pos(const QPointF &new_pos) {
    current_pos_ = new_pos;
    if (sprite_ != nullptr) {
        sprite_->sprite_->set_pos(new_pos - origin());
    }

    /// the following operations needs the map_ to be non-null (which means the entity is in a map)
    if (map_ == nullptr) {
        return;
    }

    /// update the PathingMap
    map_->remove_pathing_map(pathing_map());
    map_->add_pathing_map(pathing_map(), map_to_map(pathing_map_pos_));
    map_->update_pathing_map();

    /// update z value (lower in map -> draw higher on top)
    qreal bot = map_to_map(bounding_rect().bottomRight()).y();
    sprite_->sprite_->set_z_value(bot);

    /// if the map is in a game, let map know entity moved (watched-watching pair)
    ///  TODO: remove this, instead have game listen to when entites move
    Game *entitys_game = map_->game();
    if (entitys_game) {
        entitys_game->on_entity_moved(this);
    }

    /// if collided with something, emit
    std::unordered_set<Entity *> colliding_entities = map_->entities(this);
    for (Entity *entity : colliding_entities) {
        emit collided(this, entity);
    }

    if (current_pos_ == last_pos_) {
        emit want_to_move_but_cannot(this, last_pos_, current_pos_);
    } else {
        QPointF last_pos_cache = last_pos_;
        last_pos_ = current_pos_;
        emit map_->entity_moved(map_, this);
        emit moved(this, last_pos_cache, current_pos_);
    }
}

/// @param entitys_point: A point relative to the top_left point
void Entity::set_pos(const QPointF &entitys_point, const QPointF &to_pos) {
    QLineF line(entitys_point, origin());
    set_pos(QPointF(to_pos.x() + line.dx(), to_pos.y() + line.dy()));
}

void Entity::set_pos(std::string named_pos, const QPointF &to_pos) { set_pos(named_point(named_pos), to_pos); }

void Entity::set_x(double x) { set_pos(QPointF(x, y())); }
void Entity::set_y(double y) { set_pos(QPointF(x(), y)); }

void Entity::set_z(double z) {
    z_pos_ = z;
    if (sprite_ != nullptr) {
        scale_based_on_z();
    }
}

/// This does *not* delete the old sprite. You are responsible for the old sprite's lifetime.
void Entity::set_sprite(EntitySprite *sprite, bool auto_set_origin_and_bounding_box) {
    /// set all childrens' sprites' parent to new sprite
    for (Entity *child : children()) {
        child->sprite()->sprite_->setParentItem(sprite->sprite_);
    }

    /// make sure the new sprite is positioned correctly on the scene
    sprite->sprite_->set_pos(top_left());

    /// if the Entity is already in a map
    if (map_) {
        map_->scene()->removeItem(sprite_->sprite_);
        map_->scene()->addItem(sprite->sprite_);
        qreal bot = map_to_map(bounding_rect().bottomRight()).y();
        sprite->sprite_->set_z_value(bot);
    }

    /// auto set origin and bounding box
    /// The default result for bounding-box and origin may not be perfect.
    /// you can also set the bounding-box or origin with `Entity::set_bounding_box_and_update_origin`.
    if (auto_set_origin_and_bounding_box) {
        set_origin(QPointF(sprite->bounding_box().width() / 2, sprite->bounding_box().height() / 2));
        bounding_rect_ = sprite->bounding_box();
    }

    /// set internal sprite_ pointer to the new sprite
    sprite_ = sprite;
    scale_based_on_z();
}

void Entity::set_bounding_box_and_update_origin(const QRectF &rect) {
    bounding_rect_ = rect;
    set_origin(rect.center());
}

void Entity::add_sound(const std::string &sound_name, const std::string &file_path) {
    sound_name_to_filepath_[sound_name] = file_path;
}

void Entity::play_sound(const std::string &sound_name, int num_times_to_play) {
    assert(map_ != nullptr);

    /// do nothing if specified sound doesn't exists
    auto iter1 = sound_name_to_filepath_.find(sound_name);
    if (iter1 == sound_name_to_filepath_.end()) {
        return;
    }

    std::string file_path = iter1->second;

    auto iter2 = sound_path_to_positional_.find(file_path);
    /// create positional sound if necessary
    if (iter2 == sound_path_to_positional_.end()) {
        sound_path_to_positional_[file_path] = new PositionalSound(map_, file_path, pos());
        iter2 = sound_path_to_positional_.find(file_path);
    }

    PositionalSound *ps = iter2->second;
    ps->play(num_times_to_play);
}

void Entity::check_die(EntitySprite *sender, std::string animation) {
    if (animation != "die") {
        return;
    }
    disconnect(sender, &EntitySprite::animation_finished_completely, this, &Entity::check_die);
    deleteLater();
}

void Entity::set_facing_angle(double angle) {
    facing_angle_ = angle;
    if (sprite_) {
        sprite_->set_facing_angle(angle);
    }
}

void Entity::face_point(const QPointF &point) {
    assert(map_ != nullptr);
    QLineF line(pos(), point);
    set_facing_angle(-1 * line.angle());
}

/// Instantly moves the Entity to the specified cell in the Map.
void Entity::set_cell_pos(const Node &cell) {
    assert(map_ != nullptr);
    set_pos(map_->pathing_map().cell_to_point(cell));
}

/// TODO: move out into a non member utility function
bool Entity::can_fit(const QPointF &at_pos) {
    /// if the point is out of map, return false
    if (map_ == nullptr || !map_->contains(at_pos)) {
        return false;
    }

    // /// temporarly remove own pathing map
    map_->remove_pathing_map(pathing_map());
    map_->update_pathing_map();

    // bool can = map_->pathing_map().can_fit(pathing_map(), at_pos);
    bool can = !map_->pathing_map().filled(at_pos);

    // /// put own pathing map back
    map_->add_pathing_map(pathing_map(), map_to_map(pathing_map_pos_));
    map_->update_pathing_map();

    return can;
}

/// When a parent Entity moves, rotates, gets added to a Map, gets removed from a Map, or gets deleted,
/// so do all of its children.
void Entity::set_parent_entity(Entity *parent) {
    /// if nullptr passed in, make sure Entity has no parent
    if (parent == nullptr) {
        if (parent_ != nullptr) {
            parent_->children_.erase(this);
            parent_ = nullptr;
        }
        sprite_->sprite_->setParentItem(nullptr);
        return;
    }

    /// if already has a parent, remove parent first
    if (parent_ != nullptr) {
        set_parent_entity(nullptr);
    }
    parent_ = parent;
    parent_->children_.insert(this);
    sprite_->sprite_->setParentItem(parent->sprite()->sprite_);
}

bool Entity::has_child_recursive(Entity *entity) const {
    if (has_child(entity)) {
        return true;
    }
    for (Entity *child : children_) {
        if (child->has_child_recursive(entity)) {
            return true;
        }
    }
    return false;
}

QPointF Entity::map_to_map(const QPointF &point) const { return sprite_->sprite_->mapToScene(point); }
QPolygonF Entity::map_to_map(const QRectF &rect) const { return sprite_->sprite_->mapToScene(rect); }

QPointF Entity::named_point(std::string name) {
    /// make sure the points exists
    assert(named_points_.count(name));
    return named_points_[name];
}

void Entity::add_tag(const std::string &tag) { stl_helper::add(tags_, tag); }

bool Entity::contains_tag(const std::string &tag) { return stl_helper::contains(tags_, tag); }

void Entity::set_health(double health) {
    if (invulnerable()) {
        return;
    }
    health_ = health;
    if (health_ > max_health()) {
        health_ = max_health();
    }

    emit health_changed(this);
    if (health_ > 0) {
        return;
    }
    /// if there is no death animation, kill it directly
    if (sprite_ == nullptr || !sprite_->has_animation("die")) {
        deleteLater();
        return;
    }
    /// if there is death animation, kill it after the animation is finished.
    connect(sprite_, &EntitySprite::animation_finished_completely, this, &Entity::check_die);

    sprite_->play("die", 1, 10, 0);
    play_sound("die", 1);
}

void Entity::damage_enemy(Entity *entity, double amount) const {
    Relationship relation = Game::game->diplomacy_manager().get_relationship(group(), entity->group());
    if (relation == Relationship::ENEMY) {
        damage_entity(entity, amount);
    }
}

void Entity::damage_enemy_and_neutral(Entity *entity, double amount) const {
    Relationship relation = Game::game->diplomacy_manager().get_relationship(group(), entity->group());
    if (relation == Relationship::ENEMY || relation == Relationship::NEUTRAL) {
        damage_entity(entity, amount);
    }
}

void Entity::damage_anyone_except_children(Entity *entity, double amount) const {
    if (!has_child_recursive(entity)) {
        damage_entity(entity, amount);
    }
}

void Entity::damage_entity(Entity *entity, double amount) const {
    entity->set_health(entity->health() - amount);
    if (entity->sprite()->has_animation("hitten")) {
        entity->sprite()->play_then_go_back_to_old_animation("hitten", 1, 10, 0);
    }
}

Relationship Entity::relationship_towards(const Entity &entity) const {
    return Game::game->diplomacy_manager().get_relationship(group(), entity.group());
}

void Entity::add_slot(Slot *slot, const std::string &name) {
    slot->set_name(name);
    string_to_slot_[slot->name()] = slot;
    slot->owner_ = this;
}

Slot *Entity::slot(std::string name) {
    if (string_to_slot_.count(name) > 0) {
        return string_to_slot_[name];
    } else {
        return nullptr;
    }
}

std::unordered_set<Slot *> Entity::get_slots() {
    std::unordered_set<Slot *> all_slots;
    for (std::pair<std::string, Slot *> string_slot_pair : string_to_slot_) {
        all_slots.insert(string_slot_pair.second);
    }
    return all_slots;
}

bool Entity::equip_item(EquipableItem *item, std::string to_slot) {
    /// make sure Entity has the specified Slot
    Slot *slot_to_equip_in = slot(to_slot);
    assert(slot_to_equip_in != nullptr);
    return slot_to_equip_in->equip(item);
}

bool Entity::equip_item(EquipableItem *item, Slot *slot) { return equip_item(item, slot->name()); }

void Entity::scale_based_on_z() {
    assert(sprite_ != nullptr);
    sprite_->scale(1.0 + z_pos_ / 100.0);
}
