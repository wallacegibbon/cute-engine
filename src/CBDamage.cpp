#include "CBDamage.h"
#include "Entity.h"
#include "stl_helper.h"

using namespace cute;

void CBDamage::on_collided(Entity *entity1, Entity *entity2, const std::set<std::string> &do_not_damage_tags,
                           const std::set<Entity *> &do_not_damage_entities) {
    for (const std::string &tag : do_not_damage_tags) {
        if (entity1->contains_tag(tag) || entity2->contains_tag(tag)) {
            return;
        }
    }
    /// ignore due to specific entity
    if (stl_helper::contains_any(do_not_damage_entities, {entity1, entity2})) {
        return;
    }

    entity1->set_health(entity1->health() - health_damage_entity1_);
    entity2->set_health(entity2->health() - health_damage_entity2_);
}
