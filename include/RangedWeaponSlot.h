#pragma once

#include "RangedWeapon.h"
#include "Slot.h"

namespace cute {

class EquipableItem;

class RangedWeaponSlot : public Slot {
public:
    bool can_be_equipped(EquipableItem *item) override { return dynamic_cast<RangedWeapon *>(item); }
    void use();
};

} // namespace cute
