#include "NoTargetItem.h"
#include "NoTargetAbility.h"
#include "TopDownSprite.h"

using namespace cute;

NoTargetItem::NoTargetItem(NoTargetAbility *ability) : ability_(ability) { set_ability(ability); }

void NoTargetItem::use() {
    if (ability_) {
        ability_->set_owner(owner());
        ability_->use();
    } else {
        use_();
    }

    /// reduce number of charges
    /// -1 charges represents an infinite number of charges
    if (num_of_charges() != -1) {
        set_num_of_charges(num_of_charges() - 1);
    }
}

void NoTargetItem::set_ability(NoTargetAbility *ability) {
    ability_ = ability;
    if (ability) {
        set_icon(ability->icon());
        set_sprite(new TopDownSprite(ability->icon()));
    }
}
