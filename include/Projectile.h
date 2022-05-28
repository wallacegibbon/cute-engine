#pragma once

#include "CollisionBehavior.h"
#include "DestReachedBehavior.h"
#include "ECMover.h"
#include "Entity.h"
#include "Vendor.h"

class QTimer;

namespace cute {

/// An Entity that represents a projectile that moves a certain way and collides with things
/// along the way.
///
/// Strategy pattern is used to determine the behavior of the projectile. The
/// ECMover of the Projectile determines how the projectile moves. The
/// CollisionBehavior determines how the projectile responds when it collides
/// with Entities. The DestReachedBehavior determines what happens when the
/// Projectile reaches its destination. If you don't want the projectile
/// to collide with a certain group of entities, give that group a tag, and then
/// call add_entities_to_not_collide_with() passing in that tag. If you want the projectile to
/// collide but not damage a certain group of entities, give that group a tag and
/// call add_entities_to_not_damage() passing in that tag.
///
/// To shoot a projectile towards a certain pos use shoot_towards(). To have the
/// projectile "home" to an Entity (i.e. follow the Entity while traveling),
/// use home_towards(). home_towards() simply periodically calls shoot_towards()
/// passing in the new position of the Entity.
///
/// Example usage
/// -------------
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// Projectile* p = new SomeConcreteProjectileClass(aMover, aCollisionBehavior, listOfEntitiesToNotDmg);
/// p->add_entities_to_not_collide_with("ghost"); /// pass through all entities tagged with "ghost"
/// p->add_entities_to_not_damage("good guy"); /// collide with but do not damage all entities tagged with "good guy"
/// p->set_pos(somePos); /// set the position of the projectile
/// p->shoot_towards(someOtherPos); /// shoot the projectile towards the specified pos
/// p->home_towards(someEntity); /// periodically redirects the projectile to follow said entity
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
/// How to create your own custom Projectiles
/// -----------------------------------------
/// To create your own custom projectile, you simply need to create your own custom
/// Mover, CollisionBehavior, and DestReachedBehavior. Then give your projectile
/// these behaviors.
///
/// Note that several prebuilt Movers/CollisionBehaviors/DestReachedBehaviors
/// are included as well as several concrete Projectile subclasses that you can
/// use as examples to follow.
///
/// @author Abdullah Aghazadah
/// @date 2/21/16
class Projectile : public Entity {
    Q_OBJECT
public:
    Projectile(ECMover *mover = nullptr, CollisionBehavior *collision_behavior = nullptr,
               DestReachedBehavior *dest_reached_behavior = nullptr);

    virtual void shoot_towards(const QPointF &pos);
    void home_towards(Entity *entity);

    ECMover *move_behavior() { return mover_; }
    void set_mover(ECMover *mover);

    CollisionBehavior *collision_behavior();
    void set_collision_behavior(CollisionBehavior *collision_behavior);

    DestReachedBehavior *dest_reached_behavior();
    void set_dest_reached_behavior(DestReachedBehavior *drb);

    void add_entities_to_not_collide_with(const std::string &tag);
    void add_entity_to_not_collide_with(Entity *entity);
    void add_entities_to_not_damage(const std::string &tag);
    void add_entity_to_not_damage(Entity *entity);

public slots:
    void on_collided(Entity *self, Entity *collided_with);
    void on_succesfully_moved(ECMover *byMover);
    void on_home_step();

private:
    ECMover *mover_;
    std::unique_ptr<CollisionBehavior> collision_behavior_;
    std::unique_ptr<DestReachedBehavior> dest_reached_behavior_;

    /// ignore collisions with entities that have these tags
    std::set<std::string> do_not_collide_tags_;

    /// collide with but do not damage these entities
    std::set<std::string> do_not_damage_tags_;

    /// ignore collisions with these entities
    std::set<Entity *> do_not_collide_entities_;

    /// collide with but do not damage these entites
    std::set<Entity *> do_not_damage_entities_;

    QPointer<Entity> home_to_;
    QTimer *home_timer_;
};

} // namespace cute
