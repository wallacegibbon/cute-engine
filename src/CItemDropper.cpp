#include "CItemDropper.h"
#include "Entity.h"
#include "ItemGold.h"
#include "ItemHealthPotion.h"
#include "Map.h"
#include "Utilities.h"
#include "stl_helper.h"

using namespace cute;
using namespace stl_helper;

void CItemDropper::add_entity(Entity *entity) {
    /// do nothing if entity already added (to prevent double-listening)
    if (contains(entities_, entity)) {
        return;
    }
    /// listen to when entity dies (so we can drop an item)
    connect(entity, &Entity::dying, this, &CItemDropper::on_entity_dies);
}

void CItemDropper::remove_entity(Entity *entity) {
    /// note: don't have to handle case of entity already being removed
    /// - set handles this for us and double-unlistening is ok (does nothing)
    disconnect(entity, &Entity::dying, this, &CItemDropper::on_entity_dies);
    remove(entities_, entity);
}

void CItemDropper::on_entity_dies(Entity *entity) {
    /// do nothing if the entity is not in a map
    Map *entitys_map = entity->map();
    if (entitys_map == nullptr) {
        return;
    }

    /// other wise, drop an item
    int r = rand_int(0, 1);
    if (r == 0) {
        ItemGold *gold = new ItemGold();
        gold->set_pos(entity->pos());
        entitys_map->add_entity(gold);
    } else if (r == 1) {
        ItemHealthPotion *hp = new ItemHealthPotion(20);
        hp->set_pos(entity->pos());
        entitys_map->add_entity(hp);
    }
}
