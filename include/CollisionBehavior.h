#pragma once

#include "Vendor.h"

namespace cute {

class Entity;

/// An interface for behavior objects that decides what happens when two Entities
/// collide.
/// @author Abdullah Aghazadah
/// @date 12/8/16
class CollisionBehavior {
public:
    /// This function will be called on the CollisionBehavior whenever two entities
    /// of interest have collided. Some concrete CollisionBehaviors will damage one or both entity,
    /// some will heal, some will push one of the entities back, etc...
    /// @param entity1
    /// @param entity2 the entities that have collided.
    /// @param do_not_damage_tags Entities with these tags should not be damaged!
    /// @param do_not_damage_entities These specific entities (regarless of tags) should not be damaged!
    /// @attention Implementations of this function need to ensure that entities with any of the
    /// 'do_not_damage_tags' or entities in 'do_not_damage_entities' are in fact not damaged!
    virtual void on_collided(Entity *entity1, Entity *entity2, const std::set<std::string> &do_not_damage_tags,
                            const std::set<Entity *> &do_not_damage_entities) = 0;
};

} // namespace cute
