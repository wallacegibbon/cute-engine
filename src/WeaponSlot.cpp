#include "WeaponSlot.h"
#include "Weapon.h"

using namespace cute;

void WeaponSlot::use() {
    QLineF line(owner()->pos(), QPointF(-100, -100));
    line.setAngle(-1 * owner()->facing_angle());
    line.setLength(200);
    dynamic_cast<Weapon *>(item_)->attack(line.p2());
}
