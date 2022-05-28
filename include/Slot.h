#pragma once

#include "Vendor.h"

namespace cute {

class EquipableItem;
class Entity;

/// An abstract class that represents an equipment slot.
/// Entities can have Inventories and Slots. An entity can equip items that are in its inventory to its slots.
/// There are different types of slots, each of which can equip different types of items.
/// For example, MeleeWeapon slots can only equip MeleeWeapons.
/// @author Abdullah Aghazadah
/// @date 2/17/16
class Slot {

    /// Entity has an add_slot(Slot* s) function, that needs to update the Slot's owner_ attribute
    friend class Entity;

public:
    void set_position(const QPointF &pos) { position_ = pos; }
    QPointF position() { return position_; }

    void set_name(const std::string &s) { name_ = s; }
    std::string name() { return name_; }

    bool is_filled() { return item_ != nullptr; }

    bool equip(EquipableItem *item);
    void unequip();

    EquipableItem *item() { return item_; }

    Entity *owner() { return owner_; }

    /// Returns true if the specified Item can be equipped in this Slot.
    virtual bool can_be_equipped(EquipableItem *item) = 0;

    /// Use the item in the Slot.
    /// Suggested implementation: do something with the Item in the Slot.
    virtual void use() = 0;

protected:
    /// pos (relative to Entity that owns the Slot) that the item should go in
    QPointF position_;

    /// a string name for the slot, i.e. "rightHand", "chest", etc...
    std::string name_;

    /// item this slot houses
    EquipableItem *item_ = nullptr;

    /// who this slot belongs to
    Entity *owner_ = nullptr;
};

} // namespace cute
