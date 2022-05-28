#pragma once

#include "MeleeWeapon.h"
#include "Slot.h"

namespace cute {

class EquipableItem;

class MeleeWeaponSlot : public Slot {
public:
    bool can_be_equipped(EquipableItem *item) override { return dynamic_cast<MeleeWeapon *>(item); }
    void use();
};

} // namespace cute
