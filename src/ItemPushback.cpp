#include "ItemPushback.h"
#include "Entity.h"
#include "Inventory.h"
#include "Sprite.h"
#include "TopDownSprite.h"

using namespace cute;

ItemPushback::ItemPushback() {
    TopDownSprite *sprite = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/misc/pushback.png"));
    set_sprite(sprite);
}

void ItemPushback::use_(Entity *on_entity) {
    /// make sure the item is in an inventory
    assert(inventory() != nullptr);

    /// make sure the inventory has an owner
    Entity *owner = inventory()->owner();
    assert(owner != nullptr);

    /// move entity AMOUNT pixels along owner-entity line
    int AMOUNT = 250;
    QLineF line(owner->pos(), on_entity->pos());
    line.setLength(line.length() + AMOUNT);
    QLineF line2(on_entity->pos(), line.p2());
    on_entity->set_x(on_entity->x() + line2.dx());
    on_entity->set_y(on_entity->y() + line2.dy());
}
