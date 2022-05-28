#pragma once

#include "CollisionBehavior.h"
#include "Entity.h"
#include "Vendor.h"

namespace cute {

class CBDamage : public CollisionBehavior {
public:
    CBDamage(double health_damage_entity1, double health_damage_entity2)
            : health_damage_entity1_(health_damage_entity1), health_damage_entity2_(health_damage_entity2) {}

    void on_collided(Entity *entity1, Entity *entity2, const std::set<std::string> &do_not_damage_tags,
                     const std::set<Entity *> &do_not_damage_entities) override;

private:
    double health_damage_entity1_;
    double health_damage_entity2_;
};

} // namespace cute
