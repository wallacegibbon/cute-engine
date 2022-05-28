#pragma once

#include "Slot.h"
#include "Weapon.h"

namespace cute {

class EquipableItem;

class WeaponSlot : public Slot {
public:
    bool can_be_equipped(EquipableItem *item) override { return dynamic_cast<Weapon *>(item); };
    void use();
};

} // namespace cute
