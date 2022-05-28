#include "MeleeWeaponSlot.h"
#include "MeleeWeapon.h"

using namespace cute;

void MeleeWeaponSlot::use() {
    /// call attack on the MeleeWeapon targetting 200 in front of the owner
    MeleeWeapon *as_melee_weapon = dynamic_cast<MeleeWeapon *>(item_);
    if (as_melee_weapon == nullptr) {
        return;
    }
    QLineF line(owner()->pos(), QPointF(-100, -100));
    line.setAngle(-1 * owner()->facing_angle());
    line.setLength(200);
    as_melee_weapon->attack(line.p2());
}
