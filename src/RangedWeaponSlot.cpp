#include "RangedWeaponSlot.h"
#include "Entity.h"
#include "Inventory.h"
#include "QPointF"
#include "RangedWeapon.h"

using namespace cute;

void RangedWeaponSlot::use() {
    /// call attack of the RangeWeapon passing in a point ahead of the entity
    Entity *owner = item_->inventory()->owner();
    assert(owner != nullptr);

    int RANGE = 400; /// TODO: get from item_

    QLineF line(owner->pos(), QPointF(-5, -5));
    line.setAngle(-1 * owner->facing_angle());
    line.setLength(RANGE);

    QPointF target = line.p2();

    RangedWeapon *as_range_weapon = dynamic_cast<RangedWeapon *>(item_);
    as_range_weapon->attack(target);
}
