#include "ECItemPickerUpper.h"
#include "Inventory.h"
#include "Item.h"

using namespace cute;

ECItemPickerUpper::ECItemPickerUpper(Entity *entity) : EntityController(entity) {
    assert(entity != nullptr);
    connect(entity, &Entity::collided, this, &ECItemPickerUpper::on_collide);
}

void ECItemPickerUpper::on_collide(Entity *controlled_entity, Entity *collided_with) {
    Item *as_item = dynamic_cast<Item *>(collided_with);
    if (as_item == nullptr) {
        return;
    }
    /// do nothing if the entity does not have an inventory
    Inventory *inv = controlled_entity->inventory();
    if (inv == nullptr) {
        return;
    }
    inv->add_item(as_item);
}
