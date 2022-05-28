#include "ItemHealthPotion.h"
#include "Entity.h"
#include "Inventory.h"
#include "TopDownSprite.h"

using namespace cute;

ItemHealthPotion::ItemHealthPotion(int amount_to_heal_by_) : amount_to_heal_by_(amount_to_heal_by_) {
    TopDownSprite *s = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/misc/healthpotion.png"));
    set_sprite(s);
    set_description("A basic health potion. Heals health when used.");
}

void ItemHealthPotion::use_() {
    Entity *o = owner();
    if (o == nullptr) {
        return;
    }
    o->set_health(o->health() + amount_to_heal_by_);
}
