#include "Item.h"
#include "Inventory.h"
#include "Sprite.h"

using namespace cute;

Item::Item() : description_("Item has no description") { set_invulnerable(true); }

void Item::set_inventory(Inventory *inv) {
    if (inv == nullptr) {
        /// put item on ground
        set_pos(map_to_map(pos()));
        set_parent_entity(nullptr);
        inventory_ = nullptr;
    } else {
        /// put item in inventory
        inv->add_item(this);
    }
}

/// Sets the number of charges the Item has. -1 means infinite.
void Item::set_num_of_charges(int n) {
    num_of_charges_ = n;
    if (num_of_charges_ == 0) {
        deleteLater();
    }
    emit num_charges_changed(this);
}

/// Returns nullptr if the item isn't in an inventory or if the inventory doesn't belong to anyone.
Entity *Item::owner() {
    Inventory *inv = inventory();
    if (inv == nullptr) {
        return nullptr;
    }
    Entity *ow = inv->owner();
    return ow;
}
