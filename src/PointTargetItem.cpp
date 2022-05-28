#include "PointTargetItem.h"
#include "PointTargetAbility.h"
#include "TopDownSprite.h"

using namespace cute;

PointTargetItem::PointTargetItem(PointTargetAbility *ability) : ability_(ability) { set_ability(ability); }

void PointTargetItem::use(const QPointF &point) {
    if (ability_) {
        ability_->set_owner(owner());
        ability_->use(point);
    } else {
        use_(point);
    }

    /// reduce num charges
    /// -1 charges represents an infinite number of charges
    if (num_of_charges() != -1) {
        set_num_of_charges(num_of_charges() - 1);
    }
}

/// Sets the ability of the item.
/// Giving the item an ability means setting the icon/sprite of the item to the
/// icon of the ability and using the ability when the item is used.
void PointTargetItem::set_ability(PointTargetAbility *ability) {
    ability_ = ability;
    if (ability) {
        set_icon(ability->icon());
        set_sprite(new TopDownSprite(ability->icon()));
    }
}
