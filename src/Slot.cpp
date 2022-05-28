#include "Slot.h"
#include "Entity.h"
#include "EquipableItem.h"
#include "Inventory.h"
#include "Map.h"
#include "Sprite.h"

using namespace cute;

bool Slot::equip(EquipableItem *item) {
    assert(owner_ != nullptr);
    /// if item is not in inventory, add it
    if (!owner_->inventory()->contains(item)) {
        owner_->inventory()->add_item(item);
    }
    /// return false if the item cannot be equipped
    if (!can_be_equipped(item)) {
        return false;
    }

    /// equiped items are put into the map of the owner.
    if (owner_->map() != nullptr) {
        owner_->map()->add_entity(item);
    }

    item->set_parent_entity(owner_);
    item->set_pos(item->attachment_point(), position());
    item_ = item;
    item->slot_equipped_in_ = this;

    return true;
}

void Slot::unequip() {
    if (item_ == nullptr) {
        return;
    }
    /// remove the item from the map (will be added back when its equipped)
    item_->set_parent_entity(nullptr);
    item_->map()->remove_entity(item_);
    item_->slot_equipped_in_ = nullptr;
    item_ = nullptr;
}
