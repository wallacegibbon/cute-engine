#include "Inventory.h"
#include "Entity.h"
#include "EntityTargetItem.h"
#include "EquipableItem.h"
#include "Map.h"
#include "NoTargetItem.h"
#include "PointTargetItem.h"
#include "Sprite.h"

using namespace cute;

Inventory::~Inventory() {
    /// drop all items on the ground
    for (Item *item : items_) {
        item->set_inventory(nullptr);
    }
}

void Inventory::add_item(Item *item) {
    /// if item is already in this inventory, do nothing
    if (contains(item)) {
        return;
    }

    /// if item is already in another inventory, throw
    Inventory *items_inventory = item->inventory();
    if (items_inventory != nullptr && items_inventory != this) {
        assert(false);
    }

    /// add to this inventory
    bool stacked = false;
    for (Item *i : items_) {
        if (typeid(*i) == typeid(*item)) {
            i->set_num_of_charges(i->num_of_charges() + item->num_of_charges());
            item->deleteLater();
            stacked = true;
        }
    }
    if (!stacked) {
        items_.insert(item);
        item->inventory_ = this;
    }

    /// remove item from ground
    if (owner_ != nullptr) {
        Map *the_map = owner_->map();
        if (the_map != nullptr) {
            the_map->remove_entity(item);
        }
    }

    connect(item, &Item::dying, this, &Inventory::on_item_dying);

    emit item_added(item);
}

void Inventory::remove_item(Item *item) {
    /// Does nothing if the Item is not in the Inventory.
    if (!contains(item)) {
        return;
    }
    /// If the inventory belongs to entity who is in map
    if (owner_ != nullptr) {
        Map *the_map = owner_->map();
        if (the_map != nullptr) {
            QPointF pos_to_drop = owner_->pos();
            pos_to_drop.setY(pos_to_drop.y() + 100);
            item->set_pos(pos_to_drop);
            the_map->add_entity(item);
        }
    }
    items_.erase(item);
    item->inventory_ = nullptr;
    emit item_removed(item);
}

bool Inventory::contains(Item *item) { return items_.count(item) != 0; }

template <typename T>
std::unordered_set<T *> Inventory::get_certain_items() {
    std::unordered_set<T *> results;
    for (Item *item : items_) {
        T *converted_item = dynamic_cast<T *>(item);
        if (converted_item != nullptr) {
            results.insert(converted_item);
        }
    }
    return results;
}

std::unordered_set<EquipableItem *> Inventory::get_equipable_items() { return get_certain_items<EquipableItem>(); }

std::unordered_set<NoTargetItem *> Inventory::get_no_target_items() { return get_certain_items<NoTargetItem>(); }

std::unordered_set<EntityTargetItem *> Inventory::get_entity_target_items() {
    return get_certain_items<EntityTargetItem>();
}

std::unordered_set<PointTargetItem *> Inventory::get_point_target_items() {
    return get_certain_items<PointTargetItem>();
}

void Inventory::on_item_dying(Entity *dying_item) { remove_item(static_cast<Item *>(dying_item)); }
