#include "Projectile.h"
#include "CollisionBehavior.h"
#include "Map.h"
#include "stl_helper.h"

using namespace cute;

/// You can pass in null for any of the behaviors, but be sure to use the setters
/// to set all the behaviors prior to calling shoot_towards() or home_towards().
Projectile::Projectile(ECMover *mover, CollisionBehavior *collision_behavior,
                       DestReachedBehavior *dest_reached_behavior)
        : mover_(mover), collision_behavior_(collision_behavior), dest_reached_behavior_(dest_reached_behavior) {
    set_mover(mover);
    add_entities_to_not_collide_with("scenery");

    /// listen to when projectile collides with anything
    connect(this, &Entity::collided, this, &Projectile::on_collided);
}

/// Destroys the old Mover.
/// Please ensure that the Projectile has a Mover before calling shoot_towards() or home_towards().
void Projectile::set_mover(ECMover *mover) {
    mover_ = mover;
    if (mover == nullptr) {
        return;
    }

    /// make sure Mover operates on this entity
    mover_->set_entity(this);

    /// listen to when the projectile is done moving
    connect(mover, &ECMover::entity_succesfully_moved, this, &Projectile::on_succesfully_moved);
}

CollisionBehavior *Projectile::collision_behavior() { return collision_behavior_.get(); }

/// Sets the CollisionBehavior of the Projectile.
/// Destroys the old CollisionBehavior.
void Projectile::set_collision_behavior(CollisionBehavior *collision_behavior) {
    collision_behavior_.reset(collision_behavior);
}

DestReachedBehavior *Projectile::dest_reached_behavior() { return dest_reached_behavior_.get(); }

/// Sets the DestReachedBeahvior of the Projectile.
/// Destroys the old DestReachedBehavior.
void Projectile::set_dest_reached_behavior(DestReachedBehavior *drb) { dest_reached_behavior_.reset(drb); }

void Projectile::add_entities_to_not_collide_with(const std::string &tag) {
    stl_helper::add(do_not_collide_tags_, tag);
}

void Projectile::add_entity_to_not_collide_with(Entity *entity) { stl_helper::add(do_not_collide_entities_, entity); }

/// please check this list before damaging entities.
void Projectile::add_entities_to_not_damage(const std::string &tag) { stl_helper::add(do_not_damage_tags_, tag); }

void Projectile::add_entity_to_not_damage(Entity *entity) { stl_helper::add(do_not_damage_entities_, entity); }

void Projectile::on_collided(Entity *self, Entity *collided_with) {
    Q_UNUSED(self);

    /// this collision should be ignored
    for (const std::string &tag : do_not_collide_tags_) {
        if (collided_with->contains_tag(tag)) {
            return;
        }
    }
    for (Entity *entity : do_not_collide_entities_) {
        if (collided_with == entity) {
            return;
        }
    }
    collision_behavior_->on_collided(this, collided_with, do_not_damage_tags_, do_not_damage_entities_);
}

void Projectile::on_succesfully_moved(ECMover *byMover) {
    Q_UNUSED(byMover);
    dest_reached_behavior_->on_destination_reached(*this);
}

/// Executed periodically when home_towards() was used.
/// Will simply re-shoot the Projectile towards the Entity.
void Projectile::on_home_step() { shoot_towards(home_to_->pos()); }

/// @warning Please make sure that the behaviors are set before calling this function.
void Projectile::shoot_towards(const QPointF &pos) {
    /// assert that behaviors are set
    assert(mover_ != nullptr);
    assert(collision_behavior_ != nullptr);
    assert(dest_reached_behavior_ != nullptr);

    mover_->move_entity(pos);
}

/// This makes it so that the projectile "follows" the Entity (i.e. won't miss it).
/// @warning Please make sure that the behaviors are set before calling this function.
void Projectile::home_towards(Entity *entity) {
    /// assert that behaviors are set
    assert(mover_ != nullptr);
    assert(collision_behavior_ != nullptr);
    assert(dest_reached_behavior_ != nullptr);

    home_to_ = entity;
    home_timer_->disconnect();

    connect(home_timer_, &QTimer::timeout, this, &Projectile::on_home_step);

    /// how often to keep changing direction to follow player
    const int HOME_FREQ = 50;
    home_timer_->start(HOME_FREQ);
}
